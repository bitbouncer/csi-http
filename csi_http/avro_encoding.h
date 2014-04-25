//
// csi_http.h
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <avro/Specific.hh>
#include <avro/Encoder.hh>
#include <avro/Decoder.hh>

namespace csi
{
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

    // missing funktion in avro::StreamReader* 
    size_t readBytes(avro::StreamReader* stream, uint8_t* b, size_t n);
}; // csi

