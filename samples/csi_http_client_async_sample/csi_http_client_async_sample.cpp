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
#include "../csi_json_spirit/json_spirit.h"

struct sample_data_req1
{
    sample_data_req1() : delay(0) {}
    std::string email;
    std::string phone;
    uint64_t	delay;
};

void json_encode(const sample_data_req1& sd, json_spirit::Object& obj)
{
    obj.push_back(json_spirit::Pair("delay", sd.delay));
    obj.push_back(json_spirit::Pair("email", sd.email));
    obj.push_back(json_spirit::Pair("phone", sd.phone));
}

int main(int argc, char **argv)
{
    boost::asio::io_service io_service;
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);

    csi::http_client handler(io_service);

    /* enter io_service run loop */
    boost::thread th(boost::bind(&boost::asio::io_service::run, &io_service));

    handler.perform_async(
        csi::create_http_request(csi::http::GET, "www.dn.se", { "Accept:*/*" },
        std::chrono::milliseconds(1000)),
        [](csi::http_client::call_context::handle state)
    {
        if (state->http_result() >= 200 && state->http_result() < 300)
            BOOST_LOG_TRIVIAL(info) << "on_complete data: " << state->uri() << " got " << state->rx_content_length() << " bytes" << std::endl;
        else
            BOOST_LOG_TRIVIAL(error) << "on_complete data: " << state->uri() << " HTTPRES = " << state->http_result();
    });


    for (int i = 0; i != 1; ++i)
    {
        sample_data_req1 payload;
        payload.email = "aaaaa@domain.com";
        payload.phone = "89347856438756";
        payload.delay = 1;

        handler.perform_async(
            csi::create_json_spirit_request(csi::http::POST, "127.0.0.1:8090/rest/sample", payload, { "Content-Type:application/json", "Accept:application/json" }, std::chrono::milliseconds(1000)),
            [&payload](csi::http_client::call_context::handle request)
        {
            BOOST_LOG_TRIVIAL(info) << "perform_async POST " << request->uri() << " res " << request->http_result() << " req delay=" << payload.delay << " actual delay=" << request->milliseconds();
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(30000));
    handler.close();
    th.join();
    return 0;
}