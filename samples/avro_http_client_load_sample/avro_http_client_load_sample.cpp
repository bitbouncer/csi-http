#include <stdio.h>
#include <thread>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <csi_http/client/http_client.h>
#include <csi_http/encoding/http_rest_avro_binary_encoding.h>
#include <csi_http/encoding/http_rest_avro_json_encoding.h>
#include "../avro_defs/hello_world_request.h"
#include "../avro_defs/hello_world_response.h"

void make_calls()
{
    boost::asio::io_service io_service;
    csi::http_client handler(io_service);
    /* enter io_service run loop */
    boost::thread th(boost::bind(&boost::asio::io_service::run, &io_service));

    while (true)
    {
        sample::HelloWorldRequest request;
        request.message = "greeting to you!";
        request.delay = 0;
        auto result = handler.perform(csi::create_avro_binary_rest("127.0.0.1:8090/rest/avro_sample", request, { "Content-Type:avro/json", "Accept:avro/json" }, std::chrono::milliseconds(1000)));
        if (result->ok())
        {
            try
            {
                sample::HelloWorldResponse response;
                csi::avro_json_decode(result->rx_content(), response);
                BOOST_LOG_TRIVIAL(info) << response.message << " (" << result->rx_content_length() << " bytes) call time " << result->milliseconds() << " ms";
            }
            catch (std::exception& e)
            {
                BOOST_LOG_TRIVIAL(error) << "csi::avro_json_decode<sample::HelloWorldResponse> " << e.what();
            }
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << result->uri() << " transport_res = " << result->transport_result() << " http_res " << result->http_result();
        }
    }
}


int main(int argc, char **argv)
{
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
    for (auto i = 0; i != 100; ++i)
    {
        boost::thread th(make_calls);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300000));
    return 0;
}
