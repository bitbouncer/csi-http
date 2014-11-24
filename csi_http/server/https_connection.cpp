#include <iostream>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "https_connection.h"
#include "https_server.h"

namespace csi
{
    namespace http
    {
        https_connection::https_connection(boost::asio::io_service& io_service, https_server* server, boost::asio::ssl::context& sslcontext, const std::string& request_id_header) :
           connection(io_service, request_id_header),
           _socket(io_service, sslcontext),
           _server(server),
           _timer(io_service)
        {
            //LOG_TRACE() << "ssl_session created";
        }

        https_connection::~https_connection()
        {
            //LOG_TRACE() << "ssl_session closed";
        }

        void https_connection::start()
        {
            //initiate a timeout here
            //timer.start...

            _socket.async_handshake(boost::asio::ssl::stream_base::server,
                boost::bind(&https_connection::handle_handshake, shared_from_this(),
                boost::asio::placeholders::error));

            //LOG_TRACE() << "ssl_session::start()";
        }

        void https_connection::send_reply()
        {
            //LOG_TRACE() << "ssl_session::send_reply";
            _waiting_for_async_reply = false;
            boost::asio::async_write(_socket, _reply.to_buffers(), boost::bind(&https_connection::handle_write_complete, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void https_connection::wait_for_async_reply()
        {
            _waiting_for_async_reply = true;
            //LOG_TRACE() << "ssl_session::wait_for_async_reply";
            boost::posix_time::time_duration timeout(1, 0, 60, 0); // 5 sec for debugging
            _timer.expires_from_now(timeout);
            _timer.async_wait(boost::bind(&https_connection::handle_async_call, shared_from_this(), boost::asio::placeholders::error));
        }

        void https_connection::notify_async_reply_done()
        {
            //if (!_waiting_for_async_reply)
            //        LOG_ERROR() << "ssl_session::notify_async_reply_done : NOT WAITING...";

            //LOG_TRACE() << "ssl_session::notify_async_reply_done";
            boost::system::error_code ec;
            _timer.cancel(ec);
        }

        void https_connection::handle_handshake(const boost::system::error_code& e)
        {
            if (!e)
            {
                // insert cert check here to set up connection properties
                //LOG_TRACE() << "handshake complete";

                // fill in authentication fields in session
                //auth._is_authenticated = true;
                //auth._remote_endpoint  = socket_.lowest_layer().remote_endpoint();
                //public_key;
                //fingerprint

                _socket.async_read_some(boost::asio::buffer(_buffer), boost::bind(&https_connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            }
            else
            {
                //LOG_ERROR() << "handshake failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }

        void https_connection::handle_read(const boost::system::error_code& e, size_t bytes_transferred)
        {
            if (e == boost::asio::error::operation_aborted)
            {
                //LOG_TRACE() << "ssl_session::handle_read : operation_aborted";
                return;
            }

            if (!e)
            {
                // parser 2
                size_t nparsed = http_parse(_buffer.data(), bytes_transferred);
                bool bad_parse = (nparsed != bytes_transferred);

                // check upgrade here....

                if (bad_parse)
                {
                    _reply.create(csi::http::bad_request);
                    send_reply();
                    //LOG_WARNING() << "handle_read failed to parse http request";
                    return;
                }

                if (request_complete())
                {
                    _server->handle_request(this);
                    if (_waiting_for_async_reply)
                    {
                        boost::posix_time::time_duration timeout(1, 0, 5, 0); // wait very long since we cant be dead when async call returns
                        _timer.expires_from_now(timeout);
                        _timer.async_wait(boost::bind(&https_connection::handle_timer, shared_from_this(), boost::asio::placeholders::error));
                    }
                    else
                    {
                        send_reply();
                    }
                }
                else
                {
                    boost::posix_time::time_duration timeout(0, 0, 5, 0); // 5 sec for debugging
                    _timer.expires_from_now(timeout);
                    _timer.async_wait(boost::bind(&https_connection::handle_timer, shared_from_this(), boost::asio::placeholders::error));
                    _socket.async_read_some(boost::asio::buffer(_buffer), boost::bind(&https_connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
                }
            }
            else
            {
                //LOG_TRACE() << "ssl_session::handle_read failed with code:(" << e.message() << ") category: " << e.category().name();
            }

            // If an error occurs then no new asynchronous operations are started. This
            // means that all shared_ptr references to the connection object will
            // disappear and the object will be destroyed automatically after this
            // handler returns. The connection class's destructor closes the socket.
        }

        void https_connection::handle_write_complete(const boost::system::error_code& e, std::size_t bytes_transferred)
        {
            if (e == boost::asio::error::operation_aborted)
            {
                //LOG_TRACE() << "ssl_session::handle_write_complete : operation_aborted";
                return;
            }

            if (!e)
            {
                //LOG_TRACE() << "ssl_session::handle_write_complete " << bytes_transferred << " bytes sent";

                _reply.reset();
                //request.reset();

                boost::system::error_code ec;
                _timer.cancel(ec);

                //boost::posix_time::time_duration timeout(0,0,0,boost::posix_time::time_duration::ticks_per_second()/10);
                boost::posix_time::time_duration timeout(0, 0, 5, 0); // 5 sec for debugging
                _timer.expires_from_now(timeout);
                _timer.async_wait(boost::bind(&https_connection::handle_timer, shared_from_this(), boost::asio::placeholders::error));

                _socket.async_read_some(boost::asio::buffer(_buffer),
                    boost::bind(&https_connection::handle_read, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            }
            else
            {
                //LOG_TRACE() << "handle_write_complete failed with code:(" << e.message() << ") category: " << e.category().name();
            }


            // initiate a timeout here...

            //if connection keep alive == false
            /*
            if (!error)
            {
            // Initiate graceful connection closure.
            boost::system::error_code ignored_ec;
            socket_.shutdown(ignored_ec);
            }

            // No new asynchronous operations are started. This means that all shared_ptr
            // references to the connection object will disappear and the object will be
            // destroyed automatically after this handler returns. The connection class's
            // destructor closes the socket.
            */
        }

        void https_connection::handle_shutdown(const boost::system::error_code& e)
        {
            if (!e)
            {
                //LOG_TRACE() << "ssl_session::handle_shutdown";
                // TBD socket_.close();
            }
            else
            {
                //LOG_TRACE()  << "handle_shutdown failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }

        void https_connection::handle_timer(const boost::system::error_code& e)
        {
            if (e == boost::asio::error::operation_aborted)
            {
                return;
            }

            if (!e)
            {
                //LOG_TRACE() << "ssl_session::handle_timer";
                //socket_.cancel();
                _socket.async_shutdown(boost::bind(&https_connection::handle_shutdown, shared_from_this(), boost::asio::placeholders::error));
                //boost::system::error_code ignored_ec;
                //socket_.shutdown(ignored_ec);
            }
            else
            {
                //LOG_TRACE() << "ssl_session::handle_timer failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }

        void https_connection::handle_async_call(const boost::system::error_code& e)
        {
            //if (!_waiting_for_async_reply)
            //        LOG_ERROR() << "ssl_session::handle_async_call : NOT WAITING...";

            if (e == boost::asio::error::operation_aborted)
            {
                //LOG_TRACE() << "ssl_session::handle_async_call operation_aborted";
                send_reply();
                return;
            }

            if (!e)
            {
                //LOG_TRACE() << "ssl_session::handle_async_call";
                boost::system::error_code ignored_ec;
                _socket.async_shutdown(boost::bind(&https_connection::handle_shutdown, shared_from_this(), boost::asio::placeholders::error));
                //socket_.cancel(ignored_ec);
                //socket_.async_shutdown(boost::bind(&session::handle_shutdown, shared_from_this(), boost::asio::placeholders::error));
                //socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }
            else
            {
                //LOG_WARNING() << "ssl_session::handle_async_call failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }


        /*
        void ssl_session::handle_timeout(const boost::system::error_code& error)
        {
        if (!error)
        {
        //if not time to die them
        //timer.start...
        }
        }
        */
    };
};
