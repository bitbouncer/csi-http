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
#include <csi/base/logging/logging.h>
#include "ssl_server.h"


namespace http
{
	namespace server2 
	{
		ssl_server::ssl_server(unsigned short port, io_service_pool* pool)
			: io_service_pool_(*pool),
			acceptor_(io_service_pool_.get_io_service(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
			context_(boost::asio::ssl::context::sslv23)
		{
			context_.set_options(
				boost::asio::ssl::context::default_workarounds
				| boost::asio::ssl::context::no_sslv2
				| boost::asio::ssl::context::single_dh_use);
			context_.set_verify_mode(boost::asio::ssl::context::verify_peer);
			context_.set_password_callback(boost::bind(&ssl_server::get_password, this));
			//context_.use_certificate_chain_file("server.pem");
			//context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);

			//context_.set_verify_callback();

			//???context_.use_tmp_dh_file("dh512.pem");
			//start_accept();
			//acceptor_.bind(endpoint);

			acceptor_.listen();
			new_connection_.reset(new ssl_session(io_service_pool_.get_io_service(), request_handler_, context_));
			acceptor_.async_accept(new_connection_->socket(), boost::bind(&ssl_server::handle_accept, this, boost::asio::placeholders::error));
		}

		void ssl_server::set_certificate(const boost::filesystem::path& path)
		{
			LOG_TRACE() << "ssl_server::set_certificate " << path.generic_string();
			//context_.use_certificate_chain_file(path.generic_string());
			context_.use_certificate_file(path.generic_string(), boost::asio::ssl::context::pem);
			context_.use_private_key_file(path.generic_string(), boost::asio::ssl::context::pem);
		}

		void ssl_server::set_logging_state(bool state)
		{
			_logging = state;
			request_handler_.set_logging_state(_logging);
		}

		std::string ssl_server::get_password() const
		{
			return "test";
		}

		void ssl_server::add_request_handler(const std::string& vpath, request_handler_base* p)
		{
			request_handler_.add_request_handler(vpath, p);
		}

		/*
		void ssl_server::start_accept()
		{
		ssl_session* new_session = new ssl_session(io_service_pool_.get_io_service(), request_handler_, context_);
		acceptor_.async_accept(new_session->socket(), boost::bind(&ssl_server::handle_accept, this, new_session, boost::asio::placeholders::error));
		}
		*/

		void ssl_server::handle_accept(const boost::system::error_code& e)
		{
			if (!e)
			{
				new_connection_->start();
				new_connection_.reset(new ssl_session(io_service_pool_.get_io_service(), request_handler_, context_));
				acceptor_.async_accept(new_connection_->socket(), boost::bind(&ssl_server::handle_accept, this, boost::asio::placeholders::error));
			}
			else
			{
				LOG_TRACE() << "ssl_server::handle_accept failed with code:(" << e.message() << ") category: " << e.category().name();
			}
		}
	};
}; 
