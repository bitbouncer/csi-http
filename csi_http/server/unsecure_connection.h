//
// connection.hpp
// ~~~~~~~~~~~~~~
//
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
#include "http_parser.h"

namespace csi
{
    namespace http_server
    {
        class unsecure_server;

        /// Represents a single connection from a client.
        class unsecure_connection : public connection, public boost::enable_shared_from_this<unsecure_connection>
        {
        public:
            /// Construct a connection with the given io_service.
            unsecure_connection(boost::asio::io_service& io_service, unsecure_server* server, const std::string& request_id_header);

            ~unsecure_connection();

            /// Get the socket associated with the connection.
            boost::asio::ip::tcp::socket& socket();

            /// Start the first asynchronous operation for the connection.
            void start();

            virtual void send_reply();
            virtual void wait_for_async_reply();
            virtual void notify_async_reply_done();

        private:
            /// Handle completion of a read operation.
            void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);

            /// Handle completion of a write operation.
            void handle_write(const boost::system::error_code& e);

            void handle_timer(const boost::system::error_code& e);

            void handle_shutdown(const boost::system::error_code& e);

            //void handle_async_call(const boost::system::error_code& e);

            void handle_async_reply_done();

            boost::asio::ip::tcp::socket _socket;
            unsecure_server*			       _server;
            boost::asio::deadline_timer  _connection_timeout_timer;
            authentication				       _auth;

            /*
            void incr_count();
            void decr_count();

            static int _count;
            */
        };
        typedef boost::shared_ptr<unsecure_connection> connection_ptr;
    } // namespace server
} // namespace http

inline std::string remote_id_to_string(const csi::http_server::authentication& auth)
{
    if (auth._is_authenticated)
        return std::string("SHOULD_BE_CERT_MD5 or DN?");
    return "none";
}

inline std::string to_string(const boost::asio::ip::tcp::endpoint& ep)
{
    char buf[32];
    sprintf(buf, ":%d", (int)ep.port());
    return ep.address().to_string() + buf;
}
