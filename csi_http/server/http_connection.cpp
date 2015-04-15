//
// unsecure_connection.cpp
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// based on work by Christopher M. Kohlhoff (chris at kohlhoff dot com) same copyright as this work
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <vector>
#include <chrono>
#include <boost/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <iostream>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp> 

#include "request_handler.h"
#include "http_connection.h"
#include "http_server.h"

namespace csi
{
    namespace http
    {
        http_connection::http_connection(boost::asio::io_service& io_service, http_server* server, const std::string& request_id_header) :
            connection(io_service, request_id_header),
            _socket(io_service),
            _server(server),
            _connection_timeout_timer(io_service)
        {
        }

        http_connection::~http_connection()
        {
            boost::system::error_code ignored_ec;
            _connection_timeout_timer.cancel(ignored_ec);
            _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }

        boost::asio::ip::tcp::socket& http_connection::socket()
        {
            return _socket;
        }

        void http_connection::start()
        {
            _socket.async_read_some(boost::asio::buffer(_buffer), boost::bind(&http_connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void http_connection::send_reply()
        {
            BOOST_LOG_TRIVIAL(info) << "HTTP REQUEST END " << request().request_id() << ", " << to_string(socket().remote_endpoint()) << ", " << to_string(request().method()) << " " << request().url() << " duration " << total_microseconds() << ", status : " << reply().status() << ", " << to_string(reply().status());
            _waiting_for_async_reply = false;
            if (!keep_alive())
                reply().add(header_t("Connection", "close"));
            boost::asio::async_write(_socket, reply().to_buffers(), boost::bind(&http_connection::handle_write, shared_from_this(), boost::asio::placeholders::error));
        }

        void http_connection::wait_for_async_reply()
        {
            _waiting_for_async_reply = true;
            //connection_timeout_timer_.expires_from_now(std::chrono::milliseconds(5000)); // 5 sec for debugging
            _connection_timeout_timer.expires_from_now(boost::posix_time::time_duration(0, 0, 5, 0)); // 5 sec for debugging
            _connection_timeout_timer.async_wait(boost::bind(&http_connection::handle_timer, shared_from_this(), boost::asio::placeholders::error));
        }

        void http_connection::notify_async_reply_done()
        {
            BOOST_LOG_TRIVIAL(debug) << "http_connection::notify_async_reply_done";
            _io_service.post(boost::bind(&http_connection::handle_async_reply_done, shared_from_this()));
        }

        void http_connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
        {
            if (e == boost::asio::error::operation_aborted)
            {
                //LOG_TRACE() << "unsecure_connection::handle_read : operation_aborted";
                return;
            }

            boost::system::error_code ec;
            _connection_timeout_timer.cancel(ec);

            if (!e)
            {
                // parser 2
                size_t nparsed = http_parse(_buffer.data(), bytes_transferred);
                bool bad_parse = (nparsed != bytes_transferred);

                if (bad_parse)
                {
                    _reply.create(csi::http::bad_request);
                    send_reply();
                    //LOG_WARNING() << "handle_read failed to parse http request";
                    return;
                }

                // check upgrade here....

                if (request_complete())
                {
                    _request_start = boost::posix_time::microsec_clock::universal_time();
                    BOOST_LOG_TRIVIAL(info) << "HTTP REQUEST BEGIN " << request().request_id() << ", " << to_string(socket().remote_endpoint()) << ", " << to_string(request().method()) << " " << request().url();
                    _server->handle_request(shared_from_this());
                    if (_waiting_for_async_reply)
                    {
                        //connection_timeout_timer_.expires_from_now(std::chrono::seconds(300)); // wait very long since we cant be dead when async call returns
                        _connection_timeout_timer.expires_from_now(boost::posix_time::time_duration(1, 0, 5, 0)); // wait very long since we cant be dead when async call returns
                        _connection_timeout_timer.async_wait(boost::bind(&http_connection::handle_timer, shared_from_this(), boost::asio::placeholders::error));
                    }
                    else
                    {
                        send_reply();
                        // time session id, duration, status, uri, remote ip
                    }
                }
                else
                {
                    //connection_timeout_timer_.expires_from_now(std::chrono::milliseconds(5000)); // 5s for debugging
                    _connection_timeout_timer.expires_from_now(boost::posix_time::time_duration(0, 0, 5, 0)); // 5s for debugging
                    _connection_timeout_timer.async_wait(boost::bind(&http_connection::handle_timer, shared_from_this(), boost::asio::placeholders::error));
                    _socket.async_read_some(boost::asio::buffer(_buffer), boost::bind(&http_connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
                }
            }
            else
            {
                //LOG_DEBUG() << "handle_read failed with code:(" << e.message() << ") category: " << e.category().name();
            }

            // If an error occurs then no new asynchronous operations are started. This
            // means that all shared_ptr references to the connection object will
            // disappear and the object will be destroyed automatically after this
            // handler returns. The connection class's destructor closes the socket.
        }

        void http_connection::handle_write(const boost::system::error_code& e)
        {
            //BOOST_LOG_TRIVIAL(trace) << "handle_write";
            if (e == boost::asio::error::operation_aborted)
            {
                //LOG_TRACE() << "unsecure_connection::handle_write : operation_aborted";
                return;
            }

            if (!keep_alive())
            {
                // Initiate graceful connection closure.
                boost::system::error_code ignored_ec;
                _connection_timeout_timer.cancel(ignored_ec);
                //_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
                return;
                // No new asynchronous operations are started. This means that all shared_ptr
                // references to the connection object will disappear and the object will be
                // destroyed automatically after this handler returns. The connection class's
                // destructor closes the socket.
            }

            if (!e)
            {
                _reply.reset();
                _request.reset();

                boost::system::error_code ec;
                _connection_timeout_timer.cancel(ec);
                //connection_timeout_timer_.expires_from_now(std::chrono::milliseconds(5000));
                _connection_timeout_timer.expires_from_now(boost::posix_time::time_duration(0, 0, 5, 0));
                _connection_timeout_timer.async_wait(boost::bind(&http_connection::handle_timer, shared_from_this(), boost::asio::placeholders::error));
                _socket.async_read_some(boost::asio::buffer(_buffer), boost::bind(&http_connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            }
            else
            {
                //LOG_WARNING() << "unsecure_connection::handle_write failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }

        void http_connection::handle_shutdown(const boost::system::error_code& e)
        {
            if (!e)
            {
                //LOG_TRACE() << "unsecure_connection::handle_shutdown";
                _socket.close();
            }
            else
            {
                //LOG_WARNING() << "unsecure_connection::handle_shutdown failed with code:(" << e.message() << ") category: " << e.category().name();
            }
        }

        void http_connection::handle_timer(const boost::system::error_code& e)
        {
            if (e == boost::asio::error::operation_aborted)
                return;

            if (!e)
            {
                boost::system::error_code ignored_ec;
                _socket.cancel(ignored_ec);
                _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }
        }

        void http_connection::handle_async_reply_done()
        {
            boost::system::error_code ec;
            _connection_timeout_timer.cancel(ec);
            send_reply();
        }
    } // namespace server
} // namespace http
