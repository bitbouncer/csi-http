//
// https_connection.h
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
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "connection.h"
#include "request_handler.h"

namespace csi
{
    namespace http
    {
        class https_server;
        class https_connection : public connection, public boost::enable_shared_from_this<https_connection>
        {
        public:
            typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
            https_connection(boost::asio::io_service& io_service, https_server* server, boost::asio::ssl::context& context, const std::string& request_id_header);
            ~https_connection();

            ssl_socket::lowest_layer_type& socket() { return _socket.lowest_layer(); }
            void start();

            virtual void send_reply();
            virtual void wait_for_async_reply();
            virtual void notify_async_reply_done();
        private:
            void handle_handshake(const boost::system::error_code& error);
            void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
            void handle_write_complete(const boost::system::error_code& error, std::size_t bytes_transferred);
            void handle_timer(const boost::system::error_code& e);
            void handle_async_call(const boost::system::error_code& e);
            void handle_shutdown(const boost::system::error_code& e);

            /// Socket for the connection.
            ssl_socket	                _socket;
            https_server*			          _server;
            boost::asio::deadline_timer _timer;
        };
    };
};
