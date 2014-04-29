//
// server.cpp
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
        https_server::https_server(const std::string& address, int port, io_service_pool* pool, boost::asio::ssl::context& ssl_context, const std::string& request_id_header) :
            _io_service_pool(*pool),
            _ssl_context(ssl_context),
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
            _new_connection.reset(new https_connection(_io_service_pool.get_io_service(), this, _ssl_context, _request_id_header));
            _acceptor.async_accept(_new_connection->socket(), boost::bind(&https_server::handle_accept, this, boost::asio::placeholders::error));
        }



        //ssl_server::ssl_server(int port, io_service_pool* pool, boost::asio::ssl::context& ssl_context, const std::string& request_id_header) :
        //    io_service_pool_(*pool),
        //    _request_id_header(request_id_header),
        //    acceptor_(io_service_pool_.get_io_service(),  boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
        //{
        //    
        //    // make this a parameter TBD svante
        //    context_.set_options(
        //        boost::asio::ssl::context::default_workarounds
        //        | boost::asio::ssl::context::no_sslv2
        //        | boost::asio::ssl::context::single_dh_use);
        //    context_.set_verify_mode(boost::asio::ssl::context::verify_peer);
        //    context_.set_password_callback(boost::bind(&ssl_server::get_password, this));
        //    //context_.use_certificate_chain_file("server.pem");
        //    //context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);

        //    //context_.set_verify_callback();

        //    //???context_.use_tmp_dh_file("dh512.pem");
        //    //start_accept();
        //    //acceptor_.bind(endpoint);

        //    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        //    acceptor_.listen();
        //    new_connection_.reset(new ssl_connection(_io_service_pool.get_io_service(), this, _request_id_header));
        //    acceptor_.async_accept(new_connection_->socket(), boost::bind(&ssl_server::handle_accept, this, boost::asio::placeholders::error));
        //}

        /*
        void ssl_server::set_certificate(const boost::filesystem::path& path)
        {
            //LOG_TRACE() << "ssl_server::set_certificate " << path.generic_string();
            //context_.use_certificate_chain_file(path.generic_string());
            _ssl_context.use_certificate_file(path.generic_string(), boost::asio::ssl::context::pem);
            _ssl_context.use_private_key_file(path.generic_string(), boost::asio::ssl::context::pem);
        }
        */
 
        void https_server::handle_accept(const boost::system::error_code& e)
        {
            if (!e)
            {
                _new_connection->start();
                _new_connection.reset(new https_connection(_io_service_pool.get_io_service(), this, _ssl_context, _request_id_header));
                _acceptor.async_accept(_new_connection->socket(), boost::bind(&https_server::handle_accept, this, boost::asio::placeholders::error));
            }
            else
            {
                //LOG_TRACE() << "ssl_server::handle_accept failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }
    };
};
