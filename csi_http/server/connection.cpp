//
// connection.cpp
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/algorithm/string.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp> 
#include <boost/lexical_cast.hpp>
#include <csi_http/server/connection.h>

namespace csi
{
    namespace http_server
    {
        static http_parser_settings		s_parser_settings;

        void connection::init_parser_settings(http_parser_settings* ps)
        {
            ps->on_message_begin = connection::on_message_begin;
            ps->on_url = connection::on_url;
            ps->on_header_field = connection::on_header_field;
            ps->on_header_value = connection::on_header_value;
            ps->on_headers_complete = connection::on_headers_complete;;
            ps->on_body = connection::on_body;
            ps->on_message_complete = connection::on_message_complete;
        }

        static bool init_parser_settings()
        {
            connection::init_parser_settings(&s_parser_settings);
            return true;
        }

        static bool _init_parser = init_parser_settings();


        size_t connection::s_context_count = 0;
        csi::spinlock connection::s_spinlock;

        connection::connection(boost::asio::io_service& ios, const std::string& request_id_header) :
            _io_service(ios),
            _request_id_header_tag(request_id_header),
            _waiting_for_async_reply(false),
            _request_complete(false),
            _keep_alive(false)
        {
            {
                csi::spinlock::scoped_lock xx(s_spinlock);
                s_context_count++;
            }
            _parser.data = this; // this is the wrong "this" since this is a baseclass... dont call any virtuals from this
            http_parser_init(&_parser, HTTP_REQUEST);
        }

        connection::~connection()
        {
            {
                csi::spinlock::scoped_lock xx(s_spinlock);
                s_context_count--;
            }
       }

        size_t connection::http_parse(const char* buf, size_t len)
        {
            return http_parser_execute(&_parser, &s_parser_settings, buf, len);
        }


        const std::string& connection::request_id() const
        {
            if (_request_id.size() == 0)
            {
                // look and see if we have one in header field
                bool found = false;
                for (std::vector<header>::const_iterator i = _request._headers.begin(); i != _request._headers.end(); ++i)
                {
                    if (((*i).name == _request_id_header_tag))
                    {
                        _request_id = (*i).value;
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    boost::uuids::basic_random_generator<boost::mt19937> gen;
                    _request_id = boost::lexical_cast<std::string>(gen());
                }
            }

            return _request_id;
        }

        int connection::on_message_begin(http_parser* parser)
        {
            connection* c = (connection*)parser->data;
            c->_request.reset();
            c->_reply.reset();
            c->_request_complete = false;
            c->_last_header_was_value = false;
            c->_request._content_length = 0;
            c->_current_header_key_len = 0;
            c->_current_header_val_len = 0;
            c->_current_header_key[0] = 0;
            c->_current_header_val[0] = 0;
            return 0;
        }

        int connection::on_url(http_parser* parser, const char *at, size_t len)
        {
            connection* c = (connection*)parser->data;
            size_t nres = http_parser_parse_url(at, len, 0, &c->_parser_url);
            if (!nres)
            {
                if ((1 << UF_PATH) & c->_parser_url.field_set)
                    c->_request._url.append(&at[c->_parser_url.field_data[UF_PATH].off], c->_parser_url.field_data[UF_PATH].len);
                if ((1 << UF_QUERY) & c->_parser_url.field_set)
                    c->_request._query.append(&at[c->_parser_url.field_data[UF_QUERY].off], c->_parser_url.field_data[UF_QUERY].len);
            }
            return (int)nres;
        }

        int connection::on_header_field(http_parser* parser, const char *at, size_t len)
        {
            connection* c = (connection*)parser->data;

            if (c->_last_header_was_value)
            {
                // add current key/value to headers i request
                boost::algorithm::to_lower(c->_current_header_key);
                boost::algorithm::to_lower(c->_current_header_val);
                c->_request._headers.push_back(header(c->_current_header_key, c->_current_header_val));
                c->_current_header_key[0] = 0; // not needed
                c->_current_header_val[0] = 0; // not needed
                c->_current_header_key_len = 0;
                c->_current_header_val_len = 0;
            }

            strncpy(&(c->_current_header_key[c->_current_header_key_len]), at, len);
            c->_current_header_key_len += len;

            // keep null terminated
            c->_current_header_key[c->_current_header_key_len] = 0;
            c->_last_header_was_value = false;
            return 0;
        }

        int connection::on_header_value(http_parser* parser, const char *at, size_t len)
        {
            connection* c = (connection*)parser->data;

            // BUG kolla att vi inte skriver över 
            strncpy(&(c->_current_header_val[c->_current_header_val_len]), at, len);
            c->_current_header_val_len += len;
            // keep null terminated
            c->_current_header_val[c->_current_header_val_len] = 0;
            c->_last_header_was_value = true;

            // should we store key/val when len = 0??
            // how do we detect last key/value pair
            // or do we need to add that later???
            return 0;
        }

        int connection::on_headers_complete(http_parser* parser)
        {
            connection* c = (connection*)parser->data;

            if (c->_last_header_was_value)
            {
                boost::algorithm::to_lower(c->_current_header_key);
                boost::algorithm::to_lower(c->_current_header_val);
                c->_request._headers.push_back(header(c->_current_header_key, c->_current_header_val));
                c->_current_header_key[0] = 0; // not needed
                c->_current_header_val[0] = 0; // not needed
                c->_current_header_key_len = 0;
                c->_current_header_val_len = 0;
            }

            c->_request._content_length = (size_t)parser->content_length;

            // post and put have content....
            // check if it is zero on all other types...
            // move allocation here..

            c->_request._method = (csi::http::method_t) parser->method;

            return 0;
        }

        int connection::on_body(http_parser* parser, const char *at, size_t len)
        {
            connection* c = (connection*)parser->data;
            c->_request._avro_rx_buffer_stream_writer->writeBytes((const uint8_t*)at, len);
            c->_request._avro_rx_buffer_stream_writer->flush();
            return 0;
        }

        int connection::on_message_complete(http_parser* parser)
        {
            connection* c = (connection*)parser->data;
            c->_keep_alive = (http_should_keep_alive(parser) != 0);
            c->_request_complete = true;
            return 0;
        }
    }; // namespace
}; // namespace