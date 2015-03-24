#include <boost/algorithm/string.hpp>
#include "request.h"

namespace csi
{
    namespace http
    {
        std::string request_t::get_header(const std::string& h) const
        {
            for (std::vector<header_t>::const_iterator i = _headers.begin(); i != _headers.end(); ++i)
            {
                if (boost::iequals(h, i->name))
                    return i->value;
            }
            return "";
        }
    };
};