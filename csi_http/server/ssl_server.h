#ifndef CSI_SSL_SERVER_H
#define CSI_SSL_SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "io_service_pool.hpp"
#include "ssl_session.h"
#include "request_handler.hpp"

namespace http
{
	namespace server2 
	{
		class ssl_server
		{
		public:
			//ssl_server(unsigned short port, size_t io_service_pool_size=1);
			ssl_server(unsigned short port, io_service_pool* pool);
			std::string get_password() const;
			//void start_accept();
	
			void add_request_handler(const std::string& vpath, request_handler_base*);
			void set_logging_state(bool state);
			void set_certificate(const boost::filesystem::path&);

			/// Run the server's io_service loop.
			inline void run() { io_service_pool_.run(); }

			/// Stop the server.
			inline void stop() { io_service_pool_.stop(); }

		private:
			void handle_accept(const boost::system::error_code& error);

			/// The pool of io_service objects used to perform asynchronous operations.
			io_service_pool&				io_service_pool_;
			
			boost::asio::ip::tcp::acceptor	acceptor_;
			boost::asio::ssl::context		context_;
			
			/// The next connection to be accepted.
			ssl_session_ptr					new_connection_;

			root_request_handler			request_handler_;

			bool _logging;
		};
	};
};

#endif