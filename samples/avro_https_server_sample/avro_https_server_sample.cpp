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
#include <csi_http/server/ssl_server.h>
#include <csi_http/csi_http.h>
#include <csi_http/avro_encoding.h>
#include "../avro_defs/hello_world_request.h"
#include "../avro_defs/hello_world_response.h"

class sample_service
{
    typedef boost::asio::basic_waitable_timer<boost::chrono::steady_clock> timer;
public:
    sample_service(boost::asio::io_service& ios) : _ios(ios) {}
    virtual ~sample_service() {}

    void post(std::shared_ptr<sample::HelloWorldRequest> pd, csi::http_server::connection* context)
    {
        BOOST_LOG_TRIVIAL(info) << pd->message << ", sleeping  " << pd->delay << " ms";
        std::shared_ptr<timer> pt(new timer(_ios, boost::chrono::milliseconds(pd->delay)));
        pt->async_wait(boost::bind(&sample_service::_handle_post, this, _1, pt, pd, context));
        context->wait_for_async_reply();
    }

private:
    void _handle_post(const boost::system::error_code& ec, std::shared_ptr<timer> dummy, std::shared_ptr<sample::HelloWorldRequest> dummy2, csi::http_server::connection* context)
    {
        BOOST_LOG_TRIVIAL(info) << "done and returning ok  ";
        sample::HelloWorldResponse resp;
        resp.message = "hello to you!";
        csi::avro_binary_encode<sample::HelloWorldResponse>(resp, context->reply().content());
        context->reply().create(csi::http::ok);
        context->notify_async_reply_done();
    }

    boost::asio::io_service& _ios;
};

class sample_request_handler : public csi::http_server::request_handler
{
public:
    sample_request_handler(sample_service* ps) : _service(ps) {}
    ~sample_request_handler() {}

    /// Handle a request and produce a reply.
    virtual void handle_request(const std::string& rel_url, csi::http_server::connection* context)
    {
        if (context->request().method() == csi::http::POST)
        {
            std::shared_ptr<sample::HelloWorldRequest> request(new sample::HelloWorldRequest());
            csi::avro_binary_decode(context->request().content(), *request);
            if (request->delay == 0)
            {
                sample::HelloWorldResponse response;
                response.message = request->message;
                csi::avro_binary_encode<sample::HelloWorldResponse>(response, context->reply().content());
                context->reply().create(csi::http::ok);
            }
            else
            {
                _service->post(request, context);
            }
            return; // 
        }
        else
        {
            context->reply().create(csi::http::bad_request);
        }
    }

    sample_service* _service;
};

void print_stat(csi::http_server::request_handler* handler)
{
    uint64_t last = handler->get_no_of_requests();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        uint64_t stat = handler->get_no_of_requests();
        BOOST_LOG_TRIVIAL(info) << (stat - last) / 5.0 << " RPC/s " << " connections = " << csi::http_server::connection::connection_count();
        last = stat;
    }
}

int main(int argc, char** argv)
{
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    std::string my_address = "127.0.0.1";

    int port = 8090;
    int no_of_threads = 1;

    if (argc > 1)
        my_address = argv[1];

    if (argc > 3)
        no_of_threads = atoi(argv[3]);

    size_t last_colon = my_address.find_last_of(':');

    if (last_colon != std::string::npos)
        port = atoi(my_address.substr(last_colon + 1).c_str());

    my_address = my_address.substr(0, last_colon);

    // initialize SSL
    boost::asio::ssl::context_base::method SSL_version = boost::asio::ssl::context_base::sslv23_server;
    // load certificate files
    boost::shared_ptr<boost::asio::ssl::context> _ssl_context = boost::shared_ptr<boost::asio::ssl::context>(new boost::asio::ssl::context(SSL_version));

    _ssl_context->set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use
        );

    //_ssl_context->set_verify_mode(boost::asio::ssl::context::verify_peer);
    _ssl_context->set_password_callback([](std::size_t max_length, boost::asio::ssl::context::password_purpose purpose) -> std::string { return "nisse"; });
    //_ssl_context->use_certificate_chain_file("server.pem");
    _ssl_context->use_certificate_file("server.pem", boost::asio::ssl::context::pem);
    _ssl_context->use_private_key_file("server.pem", boost::asio::ssl::context::pem);
    //_ssl_context->use_private_key_file("server.pem", boost::asio::ssl::context::pem);

    //_ssl_context.set_verify_callback();
    //???context_.use_tmp_dh_file("dh512.pem");

    try
    {
        csi::http_server::io_service_pool	  io_pool(no_of_threads);
        sample_service					            my_service(io_pool.get_io_service());
        sample_request_handler			        my_sample_request_handler(&my_service);
        csi::http_server::https_server      s1(my_address, port, &io_pool, *_ssl_context);

        boost::thread stat_thread(boost::bind(print_stat, &my_sample_request_handler));

        s1.add_request_handler("/rest/avro_sample", &my_sample_request_handler);
        io_pool.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << " : exiting";
        return 0;
    }

    return 0;
}

