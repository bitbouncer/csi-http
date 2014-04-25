#ifndef CSI_SSL_SESSION_H
#define CSI_SSL_SESSION_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "context.hpp"
#include "request_handler.hpp"

namespace http
{
	namespace server2 
	{
		class ssl_session : public context, public boost::enable_shared_from_this<ssl_session>, private boost::noncopyable
		{
		public:
			typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

			ssl_session(boost::asio::io_service& io_service, root_request_handler& handler, boost::asio::ssl::context& context);
			~ssl_session();
			ssl_socket::lowest_layer_type& socket() { return socket_.lowest_layer(); }
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
			ssl_socket	socket_;

			/// The handler used to process the incoming request.
			root_request_handler& request_handler_;

			/// Buffer for incoming data.
			//boost::array<char, 8192> buffer_;

			/// The incoming request.
			//request request_;

			/// The parser for the incoming request.
			//request_parser request_parser_;

			/// The reply to be sent back to the client.
			//reply reply_;

			//bool	_logging;
			//enum		{ max_length = 1024 };
			//char		data_[max_length];

			boost::asio::deadline_timer timer_;

			//bool	_second; // debug only
            //authentication  auth_;
		};

		typedef boost::shared_ptr<ssl_session> ssl_session_ptr;
	};
};

#endif