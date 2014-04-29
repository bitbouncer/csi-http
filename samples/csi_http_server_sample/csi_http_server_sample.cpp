#include <iostream>
#include <strstream>
#include <iomanip>
#include <string>
#include <assert.h>
#include <thread>
#include <chrono>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/bind.hpp>
#include <csi_http/server/http_server.h>
#include <json_spirit/json_spirit.h>
#include <csi_http/json_encoding.h>

static const std::string ACCEPTS_TAG("Accept");

struct sample_data_req1
{
    sample_data_req1() : delay(0) {}
    std::string email;
    std::string phone;
    uint64_t	delay;
};

bool json_decode(const json_spirit::Object& obj, sample_data_req1& sd)
{
    sd = sample_data_req1(); // empty it.
    try
    {
        for (size_t i = 0; i != obj.size(); ++i)
        {
            const json_spirit::Pair& item = obj[i];
            if (item.name_ == "email")
                sd.email = item.value_.get_str();
            else if (item.name_ == "phone")
                sd.phone = item.value_.get_str();
            else if (item.name_ == "delay")
                sd.delay = item.value_.get_uint64();
        }
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "json_decode(json_spirit::Object, sample_data_req1) exception " << e.what() << std::endl;
        return false;
    }
    return true;
}

template<class T>
bool json_decode_object(std::istrstream& is, T& object)
{
    object = T(); // empty it.
    try
    {
        json_spirit::Value value;
        read(is, value);
        const json_spirit::Object& root = value.get_obj();
        return json_decode(root, object);
    }
    catch (std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "json_decode(std::istream&, " << typeid(T).name() << " exception: " << e.what();
        return false;
    }
}


class sample_service
{
    typedef boost::asio::basic_waitable_timer<boost::chrono::steady_clock> timer;
public:
    sample_service(boost::asio::io_service& ios) : _ios(ios) {}
    virtual ~sample_service() {}
    void post(std::shared_ptr<sample_data_req1>& pd, csi::http::connection* context)
    {
        std::shared_ptr<timer> pt(new timer(_ios, boost::chrono::milliseconds(pd->delay)));
        pt->async_wait(boost::bind(&sample_service::_handle_post, this, _1, pt, pd, context));
        context->wait_for_async_reply();
    }
private:
    void _handle_post(const boost::system::error_code& ec, std::shared_ptr<timer>&, std::shared_ptr<sample_data_req1>&, csi::http::connection* context)
    {
        context->reply().create(csi::http::ok);
        context->notify_async_reply_done();
    }
    boost::asio::io_service& _ios;
};

class sample_request_handler : public csi::http::request_handler
{
public:
    sample_request_handler(sample_service* ps) : _service(ps) {}
    ~sample_request_handler() {}

    /// Handle a request and produce a reply.
    virtual void handle_request(const std::string& rel_url, csi::http::connection* context)
    {
        //const std::vector<header>&	headers = context->headers();
        if (context->request().method() == csi::http::POST)
        {
            std::shared_ptr<sample_data_req1> req1(new sample_data_req1());
            if (csi::json_spirit_decode(context->request().content(), *req1))
                _service->post(req1, context);
            else
                context->reply().create(csi::http::bad_request);
            return;
        }
        context->reply().create(csi::http::bad_request);
    }
    sample_service* _service;
};

int main(int argc, char** argv)
{
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    std::string my_address = "127.0.0.1";

    int port = 8090;
    int no_of_threads = 4;

    if (argc > 1)
        my_address = argv[1];

    if (argc > 3)
        no_of_threads = atoi(argv[3]);

    size_t last_colon = my_address.find_last_of(':');

    if (last_colon != std::string::npos)
        port = atoi(my_address.substr(last_colon + 1).c_str());

    my_address = my_address.substr(0, last_colon);

    try
    {
        csi::http::io_service_pool	      io_pool(no_of_threads);
        sample_service					          my_service(io_pool.get_io_service());
        sample_request_handler			      my_request_handler(&my_service);
        csi::http::http_server	          s1(my_address, port, &io_pool);
        s1.add_request_handler("/rest/sample", &my_request_handler);
        io_pool.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << " : exiting";
        return 0;
    }

    return 0;
}


