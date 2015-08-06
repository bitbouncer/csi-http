#include <avro/Specific.hh>
#include <avro/Encoder.hh>
#include <avro/Decoder.hh>
#pragma once

//raw encoding without type info

namespace csi
{
    template<class T>
    void avro_raw_binary_encode(const T& src, avro::OutputStream& dst)
    {
        avro::EncoderPtr e = avro::binaryEncoder();
        e->init(dst);
        avro::encode(*e, src);
        // push back unused characters to the output stream again... really strange...                         
        // otherwise content_length will be a multiple of 4096
        e->flush();
    }

    template<class T>
    T& avro_raw_binary_decode(const avro::OutputStream& src, T& dst)
    {
        auto mis = avro::memoryInputStream(src);
        avro::DecoderPtr e = avro::binaryDecoder();
        e->init(*mis);
        avro::decode(*e, dst);
        return dst;
    }

    template<class T>
    size_t avro_raw_binary_encode(const T& src, char* buffer, size_t capacity)
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
        size_t sz = os->byteCount();
		if (sz <= capacity)
		{
			reader.readBytes((uint8_t*)buffer, sz);
			return sz;
		}
		assert(sz <= capacity);
		return 0;
    }

	template<class T>
	T& avro_raw_binary_decode(const char* buffer, size_t size, T& dst)
	{
		auto src = avro::memoryInputStream((const uint8_t*)buffer, size);
		avro::DecoderPtr e = avro::binaryDecoder();
		e->init(*src);
		avro::decode(*e, dst);
		return dst;
	}
}; // csi

