//
// reply.cpp
// ~~~~~~~~~
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <string>
#include <boost/lexical_cast.hpp>
#include "reply.h"

namespace csi {
    namespace http_server {

        namespace http_version_strings
        {
            const std::string v1_1 = "HTTP/1.1 ";
        }

        namespace status_strings {

            const std::string ok =
                "200 OK\r\n";
            const std::string created =
                "201 Created\r\n";
            const std::string accepted =
                "202 Accepted\r\n";
            const std::string no_content =
                "204 No Content\r\n";
            const std::string multiple_choices =
                "300 Multiple Choices\r\n";
            const std::string moved_permanently =
                "301 Moved Permanently\r\n";
            const std::string moved_temporarily =
                "302 Moved Temporarily\r\n";
            const std::string not_modified =
                "304 Not Modified\r\n";
            const std::string bad_request =
                "400 Bad Request\r\n";
            const std::string unauthorized =
                "401 Unauthorized\r\n";
            const std::string forbidden =
                "403 Forbidden\r\n";
            const std::string not_found =
                "404 Not Found\r\n";
            const std::string precondition_failed =
                "412 Precondition Failed\r\n";
            const std::string internal_server_error =
                "500 Internal Server Error\r\n";
            const std::string not_implemented =
                "501 Not Implemented\r\n";
            const std::string bad_gateway =
                "502 Bad Gateway\r\n";
            const std::string service_unavailable =
                "503 Service Unavailable\r\n";

            boost::asio::const_buffer to_buffer(csi::http::status_type status)
            {
                switch (status)
                {
                case csi::http::ok:
                    return boost::asio::buffer(ok);
                case csi::http::created:
                    return boost::asio::buffer(created);
                case csi::http::accepted:
                    return boost::asio::buffer(accepted);
                case csi::http::no_content:
                    return boost::asio::buffer(no_content);
                case csi::http::multiple_choices:
                    return boost::asio::buffer(multiple_choices);
                case csi::http::moved_permanently:
                    return boost::asio::buffer(moved_permanently);
                case csi::http::moved_temporarily:
                    return boost::asio::buffer(moved_temporarily);
                case csi::http::not_modified:
                    return boost::asio::buffer(not_modified);
                case csi::http::bad_request:
                    return boost::asio::buffer(bad_request);
                case csi::http::unauthorized:
                    return boost::asio::buffer(unauthorized);
                case csi::http::forbidden:
                    return boost::asio::buffer(forbidden);
                case csi::http::not_found:
                    return boost::asio::buffer(not_found);
                case csi::http::precondition_failed:
                    return boost::asio::buffer(precondition_failed);
                case csi::http::internal_server_error:
                    return boost::asio::buffer(internal_server_error);
                case csi::http::not_implemented:
                    return boost::asio::buffer(not_implemented);
                case csi::http::bad_gateway:
                    return boost::asio::buffer(bad_gateway);
                case csi::http::service_unavailable:
                    return boost::asio::buffer(service_unavailable);
                default:
                    return boost::asio::buffer(internal_server_error);
                }
            }

        } // namespace status_strings
        namespace misc_strings {

            const char name_value_separator[] = { ':', ' ' };
            const char crlf[] = { '\r', '\n' };

        } // namespace misc_strings

        std::vector<boost::asio::const_buffer> reply_t::to_buffers()
        {
            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(http_version_strings::v1_1));
            buffers.push_back(status_strings::to_buffer(status()));

            for (std::size_t i = 0; i < headers.size(); ++i)
            {
                header& h = headers[i];
                buffers.push_back(boost::asio::buffer(h.name));
                buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
                buffers.push_back(boost::asio::buffer(h.value));
                buffers.push_back(boost::asio::buffer(misc_strings::crlf));
            }

            buffers.push_back(boost::asio::buffer(misc_strings::crlf));

            size_t sz = content_length();
            if (sz)
            {
                // xpensive double copy for time beeing
                _boost_tx_buffer = std::auto_ptr<std::vector<uint8_t>>(new std::vector<uint8_t>());
                _boost_tx_buffer->reserve(sz);

                std::auto_ptr<avro::InputStream> is(avro::memoryInputStream(*_avro_tx_buffer.get()));
                avro::StreamReader stream(*is.get());

                _boost_tx_buffer->resize(sz);
                stream.readBytes(&_boost_tx_buffer->operator[](0), sz);

                buffers.push_back(boost::asio::buffer(_boost_tx_buffer->data(), sz));

                //buffers.push_back(boost::asio::buffer(contentX.data(), contentX.size()));
            }

            return buffers;
        }

        namespace stock_replies {

            const char ok[] = "";
            const char created[] =
                "<html>"
                "<head><title>Created</title></head>"
                "<body><h1>201 Created</h1></body>"
                "</html>";
            const char accepted[] =
                "<html>"
                "<head><title>Accepted</title></head>"
                "<body><h1>202 Accepted</h1></body>"
                "</html>";
            const char no_content[] =
                "<html>"
                "<head><title>No Content</title></head>"
                "<body><h1>204 Content</h1></body>"
                "</html>";
            const char multiple_choices[] =
                "<html>"
                "<head><title>Multiple Choices</title></head>"
                "<body><h1>300 Multiple Choices</h1></body>"
                "</html>";
            const char moved_permanently[] =
                "<html>"
                "<head><title>Moved Permanently</title></head>"
                "<body><h1>301 Moved Permanently</h1></body>"
                "</html>";
            const char moved_temporarily[] =
                "<html>"
                "<head><title>Moved Temporarily</title></head>"
                "<body><h1>302 Moved Temporarily</h1></body>"
                "</html>";
            const char not_modified[] =
                "<html>"
                "<head><title>Not Modified</title></head>"
                "<body><h1>304 Not Modified</h1></body>"
                "</html>";
            const char bad_request[] =
                "<html>"
                "<head><title>Bad Request</title></head>"
                "<body><h1>400 Bad Request</h1></body>"
                "</html>";
            const char unauthorized[] =
                "<html>"
                "<head><title>Unauthorized</title></head>"
                "<body><h1>401 Unauthorized</h1></body>"
                "</html>";
            const char forbidden[] =
                "<html>"
                "<head><title>Forbidden</title></head>"
                "<body><h1>403 Forbidden</h1></body>"
                "</html>";
            const char not_found[] =
                "<html>"
                "<head><title>Not Found</title></head>"
                "<body><h1>404 Not Found</h1></body>"
                "</html>";
            const char precondition_failed[] =
                "<html>"
                "<head><title>Precondition Failed</title></head>"
                "<body><h1>412 Precondition Failed</h1></body>"
                "</html>";
            const char internal_server_error[] =
                "<html>"
                "<head><title>Internal Server Error</title></head>"
                "<body><h1>500 Internal Server Error</h1></body>"
                "</html>";
            const char not_implemented[] =
                "<html>"
                "<head><title>Not Implemented</title></head>"
                "<body><h1>501 Not Implemented</h1></body>"
                "</html>";
            const char bad_gateway[] =
                "<html>"
                "<head><title>Bad Gateway</title></head>"
                "<body><h1>502 Bad Gateway</h1></body>"
                "</html>";
            const char service_unavailable[] =
                "<html>"
                "<head><title>Service Unavailable</title></head>"
                "<body><h1>503 Service Unavailable</h1></body>"
                "</html>";

            std::string to_string(csi::http::status_type status)
            {
                switch (status)
                {
                case csi::http::ok:
                    return ok;
                case csi::http::created:
                    return created;
                case csi::http::accepted:
                    return accepted;
                case csi::http::no_content:
                    return no_content;
                case csi::http::multiple_choices:
                    return multiple_choices;
                case csi::http::moved_permanently:
                    return moved_permanently;
                case csi::http::moved_temporarily:
                    return moved_temporarily;
                case csi::http::not_modified:
                    return not_modified;
                case csi::http::bad_request:
                    return bad_request;
                case csi::http::unauthorized:
                    return unauthorized;
                case csi::http::forbidden:
                    return forbidden;
                case csi::http::not_found:
                    return not_found;
                case csi::http::precondition_failed:
                    return precondition_failed;
                case csi::http::internal_server_error:
                    return internal_server_error;
                case csi::http::not_implemented:
                    return not_implemented;
                case csi::http::bad_gateway:
                    return bad_gateway;
                case csi::http::service_unavailable:
                    return service_unavailable;
                default:
                    return internal_server_error;
                }
            }

        } // namespace stock_replies

        void reply_t::create(csi::http::status_type s, const std::string& content_type, size_t extra_headers)
        {
            _status = s;
            // if we have not defined any content get a human readable string back
            // should we have a guard here agains no_content???
            size_t sz = content_length();

            if (sz == 0)
            {
                headers.resize(1 + extra_headers);
                //contentX = stock_replies::to_string(_status); if we do this then the size is not 0....
                headers[extra_headers + 0].name = "Content-Length";
                headers[extra_headers + 0].value = "0";
            }
            else
            {
                char content_length[32];
                sprintf(content_length, "%d", (int)sz);
                headers.resize(2 + extra_headers);
                headers[extra_headers + 0].name = "Content-Length";
                headers[extra_headers + 0].value = content_length;
                headers[extra_headers + 1].name = "Content-Type";
                headers[extra_headers + 1].value = content_type;
            }
        }

        void reply_t::reset()
        {
            headers.resize(0);
            _avro_tx_buffer = avro::memoryOutputStream();
            _boost_tx_buffer.reset();
        }
    } // namespace http_server
} // namespace csi
