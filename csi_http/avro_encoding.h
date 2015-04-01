//
// avro_encoding.h
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <avro/Specific.hh>
#include <avro/Encoder.hh>
#include <avro/Decoder.hh>

#pragma once

namespace csi
{
    template<class T>
    void avro_json_encode(const T& src, const avro::ValidSchema& schema, avro::OutputStream& dst)
    {
        avro::EncoderPtr e = avro::jsonEncoder(schema);
        e->init(dst);
        avro::encode(*e, src);
        // push back unused characters to the output stream again... really strange...                         
        // otherwise content_length will be a multiple of 4096
        e->flush();
    }

    template<class T>
    T& avro_json_decode(avro::InputStream& src, const avro::ValidSchema& schema, T& dst)
    {
        //avro::DecoderPtr e = avro::binaryDecoder();
        avro::DecoderPtr e = avro::jsonDecoder(schema);
        e->init(src);
        avro::decode(*e, dst);
        return dst;
    }

    // missing funktion in avro::StreamReader* 
    size_t readBytes(avro::StreamReader* stream, uint8_t* b, size_t n);

    template<class T>
    void avro_encode(const T& src, avro::OutputStream& dst)
    {
        avro::EncoderPtr e = avro::binaryEncoder();
        e->init(dst);
        avro::encode(*e, src);
        // push back unused characters to the output stream again... really strange...                         
        // otherwise content_length will be a multiple of 4096
        e->flush();
    }

    template<class T>
    T& avro_decode(avro::InputStream& src, T& dst)
    {
        avro::DecoderPtr e = avro::binaryDecoder();
        e->init(src);
        avro::decode(*e, dst);
        return dst;
    }

    template<class T>
    T& avro_decode(std::auto_ptr<avro::InputStream> src, T& dst)
    {
        avro::DecoderPtr e = avro::binaryDecoder();
        e->init(*src);
        avro::decode(*e, dst);
        return dst;
    }

    template<class T>
    size_t avro_encode(const T& src, char* buffer, size_t capacity)
    {
        //write the data
        auto os = avro::memoryOutputStream();
        avro::EncoderPtr e = avro::binaryEncoder();
        e->init(*os.get());
        avro::encode(*e, src);
        // push back unused characters to the output stream again... really strange...                         
        // otherwise content_length will be a multiple of 4096
        e->flush();

        //get the data from the internals of avro stream
        auto x = avro::memoryInputStream(*os.get());
        avro::StreamReader reader(*x.get());
        size_t content_length1 = os->byteCount();
        assert(content_length1 <= capacity);
        size_t content_length2 = csi::readBytes(&reader, (uint8_t*)buffer, capacity);
        assert(content_length1 == content_length2);
        return content_length1;
    }

    template<class T>
    T& avro_decode(const char* buffer, size_t size, T& dst)
    {
        std::auto_ptr<avro::InputStream> src = avro::memoryInputStream((const uint8_t*)buffer, size);
        avro::DecoderPtr e = avro::binaryDecoder();
        e->init(*src);
        avro::decode(*e, dst);
        return dst;
    }
}; // csi

