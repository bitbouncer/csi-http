//
// json_encoding.h
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <json_spirit/json_spirit.h>
#include <avro/Specific.hh>
#include <avro/Encoder.hh>
#include <avro/Decoder.hh>

namespace csi
{
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
    bool json_spirit_decode(avro::InputStream& src, T& dst)
    {
        dst = T(); // empty it.

        // double copy - VERY inefficient.
        // reimplement json-spirit reader to optimize this 
        // or possibly use someting else than a memorystream to hold the data... and use this buffer directly..

        avro::StreamReader stream0(src);
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

    template<class T>
    bool json_spirit_decode(const avro::OutputStream& src, T& dst)
    {
        auto mis = avro::memoryInputStream(src);
        return json_spirit_decode<T>(*mis, dst);
    }


    /*
    template<class T>
    inline bool json_spirit_decode(std::auto_ptr<avro::InputStream> src, T& dst)
    {
        return json_spirit_decode<T>(*src, dst);
    }
    */
}; // namespace