#include <thread>
#include <chrono>
#include <functional>

#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <csi_http/client/http_client.h>

#include "../avro_defs/hello_world_request.h"
#include "../avro_defs/hello_world_response.h"

int main(int argc, char **argv)
{
    boost::asio::io_service io_service;
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
    csi::http_client handler(io_service);
    /* enter io_service run loop */
    std::thread th(std::bind(static_cast<size_t(boost::asio::io_service::*)()>(&boost::asio::io_service::run), &io_service));

    while (true)
    {
        for (int i = 0; i != 100; ++i)
        {
            sample::HelloWorldRequest request;
            request.delay = 0;
            request.message = "nisse was here";
            handler.perform_async(
                csi::create_avro_json_rest(
                "127.0.0.1:8090/rest/avro_sample",
                request,
                { "Content-Type:avro/json", "Accept:avro/json" },
                std::chrono::milliseconds(1000)),
                [](csi::http_client::call_context::handle request)
            {
                BOOST_LOG_TRIVIAL(info) << "handle_post " << request->uri() << " transport_res = " << request->transport_result() << " http_res " << request->http_result();
                if (!request->ok())
                    return;
                try
                {
                    sample::HelloWorldResponse response;
                    csi::avro_json_decode(request->rx_content(), response);
                }
                catch (std::exception& e)
                {
                    BOOST_LOG_TRIVIAL(error) << "avro_json_decode " << request->uri() << " res " << request->http_result() << " actual delay=" << request->milliseconds() << " exception " << e.what();
                }
            });
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30000));
    handler.close();
    th.join();
    return 0;
}