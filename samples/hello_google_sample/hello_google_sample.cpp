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
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
    csi::http_client handler(io_service);
    /* enter io_service run loop */
    boost::thread th(boost::bind(&boost::asio::io_service::run, &io_service));

    auto result = handler.perform(csi::create_http_request(csi::http::GET, "google.com", {}, std::chrono::milliseconds(1000)), true);

    if (result->ok())
    {
        BOOST_LOG_TRIVIAL(info) << "http get: " << result->uri() << " got " << result->rx_content_length() << " bytes, time=" << result->milliseconds() << " ms";
    }
    else if (!result->transport_result())
    {
        BOOST_LOG_TRIVIAL(error) << "transport failed";
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "http get: " << result->uri() << " HTTPRES = " << result->http_result();
    }

    handler.close();
    th.join();

    return 0;
}
