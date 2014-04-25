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
        unsecure_server::unsecure_server(const std::string& address, int port, io_service_pool* pool, const std::string& request_id_header)
            : io_service_pool_(*pool),
            _request_id_header(request_id_header),
            acceptor_(io_service_pool_.get_io_service())
        {
            // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
            boost::asio::ip::tcp::resolver resolver(io_service_pool_.get_io_service());

            char port_string[32];
            sprintf(port_string, "%d", (int)port);

            boost::asio::ip::tcp::resolver::query query(address, port_string);
            boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(endpoint);
            acceptor_.listen();
            new_connection_.reset(new unsecure_connection(io_service_pool_.get_io_service(), this, _request_id_header));
            acceptor_.async_accept(new_connection_->socket(), boost::bind(&unsecure_server::handle_accept, this, boost::asio::placeholders::error));
        }

        unsecure_server::unsecure_server(int port, io_service_pool* pool, const std::string& request_id_header)
            : io_service_pool_(*pool),
            _request_id_header(request_id_header),
            acceptor_(io_service_pool_.get_io_service())
        {
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(), port);
            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_.bind(endpoint);
            acceptor_.listen();
            new_connection_.reset(new unsecure_connection(io_service_pool_.get_io_service(), this, _request_id_header));
            acceptor_.async_accept(new_connection_->socket(), boost::bind(&unsecure_server::handle_accept, this, boost::asio::placeholders::error));
        }

        void unsecure_server::run()
        {
            io_service_pool_.run();
        }

        void unsecure_server::stop()
        {
            io_service_pool_.stop();
        }

        void unsecure_server::handle_accept(const boost::system::error_code& e)
        {
            if (!e)
            {
                new_connection_->start();
                new_connection_.reset(new unsecure_connection(io_service_pool_.get_io_service(), this, _request_id_header));
                acceptor_.async_accept(new_connection_->socket(), boost::bind(&unsecure_server::handle_accept, this, boost::asio::placeholders::error));
            }
            else
            {
                //LOG_WARNING() << "server::handle_accept failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }
    } // namespace server
} // namespace csi
