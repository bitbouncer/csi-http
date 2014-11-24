//
// unsecure_connection.h
// ~~~~~~~~~~~~~~
//
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <csi_http/server/connection.h>
#include "request_handler.h"
#include <http_parser.h>

namespace csi
{
    namespace http
    {
        class http_server;

        /// Represents a single connection from a client.
        class http_connection : public connection, public boost::enable_shared_from_this<http_connection>
        {
        public:
            /// Construct a connection with the given io_service.
            http_connection(boost::asio::io_service& io_service, http_server* server, const std::string& request_id_header);

            ~http_connection();

            /// Get the socket associated with the connection.
            boost::asio::ip::tcp::socket& socket();

            /// Start the first asynchronous operation for the connection.
            void start();

            virtual void send_reply();
            virtual void wait_for_async_reply();
            virtual void notify_async_reply_done();

        private:
            void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
            void handle_write(const boost::system::error_code& e);
            void handle_timer(const boost::system::error_code& e);
            void handle_shutdown(const boost::system::error_code& e);
            void handle_async_reply_done();

            boost::asio::ip::tcp::socket _socket;
            http_server*                 _server;
            boost::asio::deadline_timer  _connection_timeout_timer;
        };
    } // namespace server
} // namespace csi

inline std::string to_string(const boost::asio::ip::tcp::endpoint& ep)
{
    char buf[32];
    sprintf(buf, ":%d", (int)ep.port());
    return ep.address().to_string() + buf;
}