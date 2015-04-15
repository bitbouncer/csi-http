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
#include <memory>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <csi_http/server/connection.h>
#include <http_parser.h>

namespace csi
{
    namespace http
    {
        class http_server;

        /// Represents a single connection from a client.
        class http_connection : public connection, public std::enable_shared_from_this<http_connection>
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
            
            inline uint32_t total_microseconds() { return ((uint32_t)(boost::posix_time::microsec_clock::universal_time() - _request_start).total_microseconds()); }

            boost::asio::ip::tcp::socket _socket;
            http_server*                 _server;
            boost::asio::deadline_timer  _connection_timeout_timer;
            boost::posix_time::ptime     _request_start;
        };
    } // namespace server
} // namespace csi

