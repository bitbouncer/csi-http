//
// unsecure_server.h
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "unsecure_connection.h"
#include "io_service_pool.h"
#include "server.h"

namespace csi
{
    namespace http_server
    {
        /// The top-level class of the HTTP server.
        class http_server : public server
        {
        public:
            /// Construct the server to listen on the specified TCP address and port
            http_server(const std::string& address, int port, io_service_pool* pool, const std::string& request_id_header = "request_id");

            http_server(int port, io_service_pool* pool, const std::string& request_id_header = "request_id");

            /// Run the server's io_service loop.
            void run();

            /// Stop the server.
            void stop();
        private:
            /// Handle completion of an asynchronous accept operation.
            void handle_accept(const boost::system::error_code& e);

            /// The pool of io_service objects used to perform asynchronous operations.
            io_service_pool&                    _io_service_pool;
            /// Acceptor used to listen for incoming connections.
            boost::asio::ip::tcp::acceptor      _acceptor;
            /// The next connection to be accepted.
            boost::shared_ptr<http_connection>  _new_connection;
            std::string                         _request_id_header;
        };
    } // namespace http_server
} // namespace csi
