#include <avro/Specific.hh>
#include <avro/Encoder.hh>
#include <avro/Decoder.hh>

#pragma once

namespace csi
{
	template<class T>
    void avro_json_encode(const T& src, avro::OutputStream& dst)
    {
        avro::EncoderPtr e = avro::jsonEncoder(*T::valid_schema());
        e->init(dst);
        avro::encode(*e, src);
        // push back unused characters to the output stream again... really strange...                         
        // otherwise content_length will be a multiple of 4096
        e->flush();
    }

    template<class T>
    T& avro_json_decode(const avro::OutputStream& src, T& dst)
    {
        auto mis = avro::memoryInputStream(src);
        avro::DecoderPtr e = avro::jsonDecoder(*T::valid_schema());
        e->init(*mis);
        avro::decode(*e, dst);
        return dst;
    }

    template<class T>
    T& avro_json_decodeEx(const avro::OutputStream& src, T& dst)
    {
        auto mis = avro::memoryInputStream(src);
        avro::DecoderPtr e = avro::jsonDecoderEx(*T::valid_schema());
        e->init(*mis);
        avro::decode(*e, dst);
        return dst;
    }

}; // csi

