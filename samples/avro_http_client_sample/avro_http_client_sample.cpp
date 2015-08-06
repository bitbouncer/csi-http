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
//#include <csi_avro/utils.h>
#include "../avro_defs/hello_world_request.h"
#include "../avro_defs/hello_world_response.h"

void handle_get(csi::http_client::call_context* state)
{
    if (state->http_result() >= 200 && state->http_result() < 300)
        BOOST_LOG_TRIVIAL(info) << "handle_get data: " << state->uri() << " got " << state->rx_content_length() << " bytes, time=" << state->milliseconds() << " ms";
    else
        BOOST_LOG_TRIVIAL(error) << "handle_get data: " << state->uri() << " HTTPRES = " << state->http_result();
}

int main(int argc, char **argv)
{
    boost::asio::io_service io_service;
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
    csi::http_client handler(io_service);
    /* enter io_service run loop */
    boost::thread th(boost::bind(&boost::asio::io_service::run, &io_service));

    for (auto i = 0; i != 1000; ++i)
    {
        sample::HelloWorldRequest request;
        request.message = "greeting to you!";
        request.delay = 0;
        auto result = handler.perform(csi::create_avro_json_rest("127.0.0.1:8090/rest/avro_sample", request, { "Content-Type:avro/json", "Accept:avro/json" }, std::chrono::milliseconds(1000)));
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
                BOOST_LOG_TRIVIAL(error) << "csi::avro_json_decode<sample::HelloWorldResponse> " << e.what() << " content:" << debug_to_string(result->rx_content());
            }
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << result->uri() << " transport_res = " << result->transport_result() << " http_res " << result->http_result();
        }
    }
    handler.close();
    th.join();

    return 0;
}
