#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "https_connection.h"
#include "server.h"

namespace csi
{
    namespace http
    {
        class https_server : public server
        {
        public:
            https_server(boost::asio::io_service& ios, const std::string& address, int port, boost::asio::ssl::context& ssl_context, const std::string& request_id_header = "request_id");
            https_server(boost::asio::io_service& ios, int port, boost::asio::ssl::context& ssl_context, const std::string& request_id_header = "request_id");
            /// Run the server's io_service loop.
            //inline void run() { _io_service_pool.run(); }
            /// Stop the server.
            //inline void stop() { _io_service_pool.stop(); }
        private:
            void handle_accept(const boost::system::error_code& error);
            boost::asio::io_service&            _ios;
            boost::asio::ssl::context&          _ssl_context;
            boost::asio::ip::tcp::acceptor      _acceptor;
            boost::shared_ptr<https_connection> _new_connection;
            std::string                         _request_id_header;
        };
    };
};
