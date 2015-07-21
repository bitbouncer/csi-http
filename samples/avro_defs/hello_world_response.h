/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef HELLO_WORLD_RESPONSE_H_2254600785__H_
#define HELLO_WORLD_RESPONSE_H_2254600785__H_


#include <sstream>
#include <boost/any.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/make_shared.hpp>
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "avro/Compiler.hh"

namespace sample {
struct HelloWorldResponse {
    std::string message;
    HelloWorldResponse() :
        message(std::string())
        { }
//  avro extension
    static inline const boost::uuids::uuid      schema_hash()      { static const boost::uuids::uuid _hash(boost::uuids::string_generator()("c6b2babc-4110-9228-5e33-871766c7259e")); return _hash; }
    static inline const char*                   schema_as_string() { return "{\"type\":\"record\",\"namespace\":\"se.csi.sample\",\"name\":\"HelloWorldResponse\",\"fields\":[{\"name\":\"message\",\"type\":\"string\"}]}"; } 
    static boost::shared_ptr<avro::ValidSchema> valid_schema()     { static const boost::shared_ptr<avro::ValidSchema> _validSchema(boost::make_shared<avro::ValidSchema>(avro::compileJsonSchemaFromString(schema_as_string()))); return _validSchema; }
};

}
namespace avro {
template<> struct codec_traits<sample::HelloWorldResponse> {
    static void encode(Encoder& e, const sample::HelloWorldResponse& v) {
        avro::encode(e, v.message);
    }
    static void decode(Decoder& d, sample::HelloWorldResponse& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.message);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.message);
        }
    }
};

}
#endif
