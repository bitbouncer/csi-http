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
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace sample {
struct HelloWorldResponse {
    std::string message;
    HelloWorldResponse() :
        message(std::string())
        { }
};

}
namespace avro {
template<> struct codec_traits<sample::HelloWorldResponse> {
    static void encode(Encoder& e, const sample::HelloWorldResponse& v) {
        avro::encode(e, v.message);
    }
    static void decode(Decoder& d, sample::HelloWorldResponse& v) {
        avro::decode(d, v.message);
    }
};

}
#endif
