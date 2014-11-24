// reply.h
// ~~~~~~~~~
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <avro/Specific.hh>
#include <csi_http/csi_http.h>
#include "header.h"

namespace csi
{
    namespace http
    {
        /// A reply to be sent to a client.
        class reply_t : public boost::noncopyable
        {
        public:
            inline csi::http::status_type status() const { return _status; }

            inline avro::OutputStream&     content()       { return *_avro_tx_buffer.get(); }
            inline size_t           content_length() const { return _avro_tx_buffer->byteCount(); }

            /// Get a stock reply.
            void create(csi::http::status_type status, const std::string& content_type = "text/html", size_t extra_headers = 0);
            void reset();

            /// Convert the reply into a vector of buffers. The buffers do not own the
            /// underlying memory blocks, therefore the reply object must remain valid and
            /// not be changed until the write operation has completed.
            std::vector<boost::asio::const_buffer> to_buffers();

            /// The headers to be included in the reply.
            std::vector<header>                    headers;
            /// The content to be sent in the reply.
            std::auto_ptr<avro::OutputStream>      _avro_tx_buffer;
            std::auto_ptr<std::vector<uint8_t>>    _boost_tx_buffer; // expensive double buffer - to be fixed.
        private:
            csi::http::status_type _status;
        };
    } // namespace server
} // namespace csi
