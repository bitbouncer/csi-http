//
// csi_http.cpp
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/lexical_cast.hpp>
#include "csi_http.h"


std::string to_string(csi::http::status_type s)
{
    switch (s)
    {
    case csi::http::ok:                        return "ok";
    case csi::http::created:                   return "created";
    case csi::http::accepted:                  return "accepted";
    case csi::http::no_content:                return "no_content";
    case csi::http::multiple_choices:          return "multiple_choices";
    case csi::http::moved_permanently:         return "moved_permanently";
    case csi::http::moved_temporarily:         return "moved_temporarily";
    case csi::http::not_modified:              return "not_modified";
    case csi::http::bad_request:               return "bad_request";
    case csi::http::unauthorized:              return "unauthorized";
    case csi::http::forbidden:                 return "forbidden";
    case csi::http::not_found:                 return "not_found";
    case csi::http::precondition_failed:       return "precondition_failed";
    case csi::http::internal_server_error:     return "internal_server_error";
    case csi::http::not_implemented:           return "not_implemented";
    case csi::http::bad_gateway:               return "bad_gateway";
    case csi::http::service_unavailable:       return "service_unavailable";
    default:                                   return boost::lexical_cast<std::string>((long)s);
    };
};

static std::string method_strings[csi::http::PURGE + 1]
{
    "DELETE",
        "GET",
        "HEAD",
        "POST",
        "PUT",
        /* pathological */
        "CONNECT",
        "OPTIONS",
        "TRACE",
        /* webdav */
        "COPY",
        "LOCK",
        "MKCOL",
        "MOVE",
        "PROPFIND",
        "PROPPATCH",
        "SEARCH",
        "UNLOCK",
        /* subversion */
        "REPORT",
        "MKACTIVITY",
        "CHECKOUT",
        "MERGE",
        /* upnp */
        "MSEARCH",
        "NOTIFY",
        "SUBSCRIBE",
        "UNSUBSCRIBE",
        /* RFC-5789 */
        "PATCH",
        "PURGE"
};

const std::string& to_string(csi::http::method_t e)
{
    return method_strings[e];
}


std::string debug_to_string(const avro::OutputStream& os)
{
    std::string res;
    size_t sz = os.byteCount();
    res.reserve(sz);
    res.resize(sz);
    auto x = avro::memoryInputStream(os);
    avro::StreamReader reader(*x.get());
    //size_t actual = csi::readBytes(&reader, (uint8_t*)&res[0], sz);
    reader.readBytes((uint8_t*)&res[0], sz);
    //assert(actual == sz);
    return res;
}
;