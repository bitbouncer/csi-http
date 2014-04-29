//
// unsecure_server.cpp
//
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "unsecure_server.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace csi
{
    namespace http_server
    {
        http_server::http_server(const std::string& address, int port, io_service_pool* pool, const std::string& request_id_header)
            : _io_service_pool(*pool),
            _request_id_header(request_id_header),
            _acceptor(_io_service_pool.get_io_service())
        {
            // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
            boost::asio::ip::tcp::resolver resolver(_io_service_pool.get_io_service());

            char port_string[32];
            sprintf(port_string, "%d", (int)port);

            boost::asio::ip::tcp::resolver::query query(address, port_string);
            boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
            _acceptor.open(endpoint.protocol());
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.bind(endpoint);
            _acceptor.listen();
            _new_connection.reset(new http_connection(_io_service_pool.get_io_service(), this, _request_id_header));
            _acceptor.async_accept(_new_connection->socket(), boost::bind(&http_server::handle_accept, this, boost::asio::placeholders::error));
        }

        http_server::http_server(int port, io_service_pool* pool, const std::string& request_id_header)
            : _io_service_pool(*pool),
            _request_id_header(request_id_header),
            _acceptor(_io_service_pool.get_io_service())
        {
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(), port);
            _acceptor.open(endpoint.protocol());
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.bind(endpoint);
            _acceptor.listen();
            _new_connection.reset(new http_connection(_io_service_pool.get_io_service(), this, _request_id_header));
            _acceptor.async_accept(_new_connection->socket(), boost::bind(&http_server::handle_accept, this, boost::asio::placeholders::error));
        }

        void http_server::run()
        {
            _io_service_pool.run();
        }

        void http_server::stop()
        {
            _io_service_pool.stop();
        }

        void http_server::handle_accept(const boost::system::error_code& e)
        {
            if (!e)
            {
                _new_connection->start();
                _new_connection.reset(new http_connection(_io_service_pool.get_io_service(), this, _request_id_header));
                _acceptor.async_accept(_new_connection->socket(), boost::bind(&http_server::handle_accept, this, boost::asio::placeholders::error));
            }
            else
            {
                //LOG_WARNING() << "server::handle_accept failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }
    } // namespace server
} // namespace csi
