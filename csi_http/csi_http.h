//
// csi_http.h
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <strstream>
#include <chrono>
#include <stdint.h>
#include <boost/function.hpp>
#include <iostream>

#include <avro/Specific.hh>
#include <avro/Encoder.hh>
#include <avro/Decoder.hh>

namespace csi
{
    namespace http
    {
        // taken from http://httpstatus.es/
        enum status_type
        {
            undefined = 0,

            //1xx informational
            continue_with_request = 100,
            switching_protocols = 101,
            processing = 102,
            checkpoint = 103,
            uri_to_long = 122,

            //2xx success
            ok = 200,
            created = 201,
            accepted = 202,
            processed_non_authorative = 203,
            no_content = 204,
            reset_content = 205,
            partial_content = 206,
            multi_status = 207,
            already_reported = 208,
            im_used = 226,

            //3xx redirection
            multiple_choices = 300,
            moved_permanently = 301,
            moved_temporarily = 302,
            see_other = 303, // ??? bad name....
            not_modified = 304,
            use_proxy = 305,
            switch_proxy = 306,
            temporary_redirect = 307,
            resume_incomplete = 308,

            //4xx client error
            bad_request = 400,
            unauthorized = 401,
            forbidden = 403,
            not_found = 404,
            precondition_failed = 412,

            //5xx server error
            internal_server_error = 500,
            not_implemented = 501,
            bad_gateway = 502,
            service_unavailable = 503
        };

        // must be equal to the one in http_parser.h
        enum method_t
        {
            DELETE_ = 0, /* without underscore clashed with macro */
            GET = 1,
            HEAD = 2,
            POST = 3,
            PUT = 4,
            /* pathological */
            CONNECT = 5,
            OPTIONS = 6,
            TRACE = 7,
            /* webdav */
            COPY = 8,
            LOCK = 9,
            MKCOL = 10,
            MOVE = 11,
            PROPFIND = 12,
            PROPPATCH = 13,
            SEARCH = 14,
            UNLOCK = 15,
            /* subversion */
            REPORT = 16,
            MKACTIVITY = 17,
            CHECKOUT = 18,
            MERGE = 19,
            /* upnp */
            MSEARCH = 20,
            NOTIFY = 21,
            SUBSCRIBE = 22,
            UNSUBSCRIBE = 23,
            /* RFC-5789 */
            PATCH = 24,
            PURGE = 25
        };

    }

    template<class T>
    void avro_json_encode(const T& src, avro::OutputStream& dst)
    {
        avro::EncoderPtr e = avro::jsonEncoder()
            e->init(dst);
        avro::encode(*e, src);
        // push back unused characters to the output stream again... really strange... 			
        // otherwise content_length will be a multipple of 4096
        e->flush();
    }

    template<class T>
    T& avro_json_decode(avro::InputStream& src, T& dst)
    {
        avro::DecoderPtr e = avro::binaryDecoder();
        e->init(src);
        avro::decode(*e, dst);
        return dst;
    }

    template<class T>
    void avro_binary_encode(const T& src, avro::OutputStream& dst)
    {
        avro::EncoderPtr e = avro::binaryEncoder();
        e->init(dst);
        avro::encode(*e, src);
        // push back unused characters to the output stream again... really strange... 			
        // otherwise content_length will be a multipple of 4096
        e->flush();
    }

    template<class T>
    T& avro_binary_decode(avro::InputStream& src, T& dst)
    {
        avro::DecoderPtr e = avro::binaryDecoder();
        e->init(src);
        avro::decode(*e, dst);
        return dst;
    }

    template<class T>
    T& avro_binary_decode(std::auto_ptr<avro::InputStream>& src, T& dst)
    {
        avro::DecoderPtr e = avro::binaryDecoder();
        e->init(*src);
        avro::decode(*e, dst);
        return dst;
    }


    // naive implementation... 
    // double copy - first to string and then to buffer
    template<class T>
    void json_spirit_encode(const T& src, avro::OutputStream& dst)
    {
        json_spirit::Object root_object;
        json_encode(src, root_object);
        //double copy... reimplement json-spirit writer...
        std::string s = write(root_object); // options can be added  for debugging .. prettyprint
        avro::StreamWriter writer(dst);
        writer.writeBytes((const uint8_t*)s.data(), s.size());
    }

    // naive implementation... 
    // double copy - first to std::stringstream and then to object
    template<class T>
    bool json_spirit_decode(std::auto_ptr<avro::InputStream>& src, T& dst)
    {
        dst = T(); // empty it.

        // double copy - VERY inefficient.
        // reimplement json-spirit reader to optimize this 
        // or possibly use someting else than a memorystream to hold the data... and use this buffer directly..

        avro::StreamReader stream0(*src.get());
        std::stringstream  stream1;

        while (stream0.hasMore())
        {
            uint8_t ch = stream0.read();
            stream1.write((const char*)&ch, 1);
        }

        try
        {
            json_spirit::Value value;
            read(stream1, value);
            const json_spirit::Object& root = value.get_obj();
            return json_decode(root, dst);
        }
        catch (std::exception& e)
        {
            BOOST_LOG_TRIVIAL(error) << "json_decode(std::istream&, " << typeid(T).name() << " exception: " << e.what();
            return false;
        }
    }

    // missing funktion in avro::StreamReader* 
    size_t readBytes(avro::StreamReader* stream, uint8_t* b, size_t n);
}; // namespace csi

std::string to_string(csi::http::status_type s);
const std::string& to_string(csi::http::method_t s);
std::string uri_encode(const std::string& str);

