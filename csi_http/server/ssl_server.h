#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "io_service_pool.h"
#include "ssl_connection.h"
#include "server.h"

namespace csi
{
    namespace http_server
    {
        class ssl_server : public server
        {
        public:
            ssl_server(const std::string& address, int port, io_service_pool* pool, boost::asio::ssl::context& ssl_context, const std::string& request_id_header = "request_id");
            ssl_server(int port, io_service_pool* pool, boost::asio::ssl::context& ssl_context, const std::string& request_id_header = "request_id");

            std::string get_password() const;

            void set_certificate(const boost::filesystem::path&);

            /// Run the server's io_service loop.
            inline void run() { _io_service_pool.run(); }

            /// Stop the server.
            inline void stop() { _io_service_pool.stop(); }

        private:
            void handle_accept(const boost::system::error_code& error);

            io_service_pool&				        _io_service_pool;
            boost::asio::ssl::context&	    _ssl_context;
            boost::asio::ip::tcp::acceptor	_acceptor;
            ssl_session_ptr					        _new_connection;
            std::string                     _request_id_header;
        };
    };
};
