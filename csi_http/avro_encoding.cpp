//
// csi_http.cpp
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "avro_encoding.h"

namespace csi
{
    size_t readBytes(avro::StreamReader* stream, uint8_t* b, size_t n) {
        size_t total = 0;
        while (n > 0) {
            if (stream->next_ == stream->end_) {
                if (!stream->fill())
                    return total;
            }
            size_t q = stream->end_ - stream->next_;
            if (q > n) {
                q = n;
            }
            ::memcpy(b, stream->next_, q);
            stream->next_ += q;
            b += q;
            n -= q;
            total += q;
        }
        return total;
    }
};
