//
// https_server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "https_server.h"

namespace csi
{
    namespace http
    {
        https_server::https_server(boost::asio::io_service& ios, const std::string& address, int port, boost::asio::ssl::context& ssl_context, const std::string& request_id_header) :
            _ios(ios),
            _ssl_context(ssl_context),
            _request_id_header(request_id_header),
            _acceptor(_ios)
        {
            // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
            boost::asio::ip::tcp::resolver resolver(_ios);

            char port_string[32];
            sprintf(port_string, "%d", (int)port);

            boost::asio::ip::tcp::resolver::query query(address, port_string);
            boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
            
            _acceptor.open(endpoint.protocol());
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.bind(endpoint);
            _acceptor.listen();
            _new_connection.reset(new https_connection(_ios, this, _ssl_context, _request_id_header));
            _acceptor.async_accept(_new_connection->socket(), boost::bind(&https_server::handle_accept, this, boost::asio::placeholders::error));
        }

        void https_server::handle_accept(const boost::system::error_code& e)
        {
            if (!e)
            {
                _new_connection->start();
                _new_connection.reset(new https_connection(_ios, this, _ssl_context, _request_id_header));
                _acceptor.async_accept(_new_connection->socket(), boost::bind(&https_server::handle_accept, this, boost::asio::placeholders::error));
            }
            else
            {
                //LOG_TRACE() << "ssl_server::handle_accept failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }
    };
};
