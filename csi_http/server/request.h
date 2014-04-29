//
// reply.h
// ~~~~~~~~~
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#include <vector>
#include "header.h"
#include <avro/Specific.hh>
#include <csi_http/csi_http.h>

namespace csi
{
    namespace http
    {
        /// A request to from a client
        class request_t : public boost::noncopyable
        {
        public:
            request_t() { reset(); }

            void reset()
            {
                _content_length = 0;
                _url.clear();
                _query.clear();
                _headers.clear();
                _avro_rx_buffer = avro::memoryOutputStream();
                _avro_rx_buffer_stream_writer = std::auto_ptr<avro::StreamWriter>(new avro::StreamWriter(*_avro_rx_buffer));
            }

            inline size_t content_length() const {
                assert((_content_length > 0) ? _avro_rx_buffer->byteCount() == _content_length : true);
                return _avro_rx_buffer->byteCount();
            }

            inline csi::http::method_t method() const               { return _method; }
            inline std::auto_ptr<avro::InputStream> content() const { return avro::memoryInputStream(*_avro_rx_buffer); }
            inline const std::vector<header>&			  headers() const { return _headers; }
            inline const std::string& url() const                   { return _url; }
            inline const std::string& query() const	                { return _query; }

            csi::http::method_t                _method;
            std::string                        _url;
            std::string                        _query;
            std::vector<header>							   _headers;
            size_t                             _content_length;
            std::auto_ptr<avro::OutputStream>	 _avro_rx_buffer;
            std::auto_ptr<avro::StreamWriter>	 _avro_rx_buffer_stream_writer;
        };
    };
};