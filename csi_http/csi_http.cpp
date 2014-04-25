#include <boost/lexical_cast.hpp>
#include "csi_http.h"

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

std::string to_string(csi::http::status_type s)
{
    switch (s)
    {
    case csi::http::ok:						return "ok";
    case csi::http::created:				return "created";
    case csi::http::accepted:				return "accepted";
    case csi::http::no_content:				return "no_content";
    case csi::http::multiple_choices:		return "multiple_choices";
    case csi::http::moved_permanently:		return "moved_permanently";
    case csi::http::moved_temporarily:		return "moved_temporarily";
    case csi::http::not_modified:			return "not_modified";
    case csi::http::bad_request:			return "bad_request";
    case csi::http::unauthorized:			return "unauthorized";
    case csi::http::forbidden:				return "forbidden";
    case csi::http::not_found:				return "not_found";
    case csi::http::precondition_failed:	return "precondition_failed";
    case csi::http::internal_server_error:	return "internal_server_error";
    case csi::http::not_implemented:		return "not_implemented";
    case csi::http::bad_gateway:			return "bad_gateway";
    case csi::http::service_unavailable:	return "service_unavailable";
    default: return boost::lexical_cast<std::string>((long)s);
    };
};

static std::string method_strings[csi::http::PURGE]
{
"DELETE",
"GET",
"HEAD"
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
    return 	method_strings[e];
}

//int csi::http_request_state::_count=0;

