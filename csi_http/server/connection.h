//
// connection.h
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "request.h"
#include "reply.h"
#include <http_parser.h>
#include <csi_http/spinlock.h>

namespace csi
{
    namespace http
    {
        class connection : private boost::noncopyable
        {
            enum { MAX_HEADER_SIZE = 4096 };

        public:
            connection(boost::asio::io_service& ios, const std::string& request_id_header); // server agent
            virtual ~connection();

            virtual void wait_for_async_reply() = 0;
            virtual void notify_async_reply_done() = 0;

            inline const request_t& request() const { return _request; }
            inline reply_t&         reply()         { return _reply; }
            const std::string&      request_id() const;
            inline bool             waiting_for_reply() const   { return _waiting_for_async_reply; }
            size_t                  http_parse(const char* at, size_t len);
            static void             init_parser_settings(http_parser_settings* ps);
            inline static size_t    connection_count() { return s_context_count; }
        protected:
            virtual void            send_reply() = 0;
            inline bool             request_complete() const                 { return _request_complete; }
            inline bool             keep_alive() const                 { return _keep_alive; }
            inline bool             upgrade() const                               { return (_parser.upgrade > 0); }
            static int              on_message_begin(http_parser*);
            static int              on_url(http_parser*, const char *at, size_t len);
            static int              on_header_field(http_parser*, const char *at, size_t len);
            static int              on_header_value(http_parser*, const char *_last_header_was_valueat, size_t len);
            static int              on_headers_complete(http_parser*);
            static int              on_body(http_parser*, const char *_last_header_was_valueat, size_t len);
            static int              on_message_complete(http_parser*);

            boost::asio::io_service&   _io_service;
            const std::string          _request_id_header_tag;
            http_parser                _parser;
            boost::array<char, 8192>   _buffer; /// Buffer for incoming data.
            request_t                  _request;
            reply_t                    _reply;
            bool                       _last_header_was_value;
            char                       _current_header_key[MAX_HEADER_SIZE];
            char                       _current_header_val[MAX_HEADER_SIZE];
            size_t                     _current_header_key_len;
            size_t                     _current_header_val_len;
            http_parser_url            _parser_url;
            bool                       _request_complete;
            bool                       _keep_alive;
            mutable std::string        _request_id; /* assigned a uuid if needed and none was given in the request */
            bool                       _waiting_for_async_reply;
            static csi::spinlock       s_spinlock;
            static size_t              s_context_count;
        };
    };
};
