//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_server_REPLY_HPP
#define HTTP_server_REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include "header.h"
#include <csi_http/csi_http.h>

namespace csi
{
    namespace http_server
    {

        /// A reply to be sent to a client.
        class reply_t : public boost::noncopyable
        {
        public:
            /// The status of the reply.
            enum status_type
            {
                ok = 200,
                created = 201,
                accepted = 202,
                no_content = 204,
                multiple_choices = 300,
                moved_permanently = 301,
                moved_temporarily = 302,
                not_modified = 304,
                bad_request = 400,
                unauthorized = 401,
                forbidden = 403,
                not_found = 404,
                not_acceptable = 406,
                precondition_failed = 412,
                internal_server_error = 500,
                not_implemented = 501,
                bad_gateway = 502,
                service_unavailable = 503
            };


            inline status_type status() const { return _status; }

            avro::OutputStream&     content() { return *_avro_tx_buffer.get(); }
            inline size_t           content_length() const { return _avro_tx_buffer->byteCount(); }


            /// The headers to be included in the reply.
            std::vector<header> headers;

            /// The content to be sent in the reply.
            std::auto_ptr<avro::OutputStream>			_avro_tx_buffer;
            std::auto_ptr<std::vector<uint8_t>>		_boost_tx_buffer; // expensive double buffer - to be fixed.

            /// Convert the reply into a vector of buffers. The buffers do not own the
            /// underlying memory blocks, therefore the reply object must remain valid and
            /// not be changed until the write operation has completed.
            std::vector<boost::asio::const_buffer> to_buffers();

            /// Get a stock reply.
            //static reply stock_reply(status_type status);
            void create(status_type status, const std::string& content_type = "text/html", size_t extra_headers = 0);
            void reset();
        private:
            status_type _status;
        };

    } // namespace server
} // namespace http

#endif // HTTP_server_REPLY_HPP
