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
#include <boost/asio.hpp>
#include "http_connection.h"
#include "server.h"

namespace csi
{
    namespace http
    {
        /// The top-level class of the HTTP server.
        class http_server : public server<http_connection>
        {
        public:
            /// Construct the server to listen on the specified TCP address and port
            http_server(boost::asio::io_service& ios, const std::string& address, int port, const std::string& request_id_header = "request_id");
            http_server(boost::asio::io_service& ios, int port, const std::string& request_id_header = "request_id");
        private:
            /// Handle completion of an asynchronous accept operation.
            void handle_accept(const boost::system::error_code& e);

            boost::asio::io_service&            _ios;
            /// Acceptor used to listen for incoming connections.
            boost::asio::ip::tcp::acceptor      _acceptor;
            /// The next connection to be accepted.
            std::shared_ptr<http_connection>    _new_connection;
            std::string                         _request_id_header;
        };
    } // namespace http
} // namespace csi
