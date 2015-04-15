#include "uri_pattern.h"
#include <boost/algorithm/string.hpp>
namespace csi
{
    uri_pattern::uri_pattern(const std::string& str) : _catch_all(false)
    {
        boost::split(_parts, str, boost::is_any_of("/"));
        if (_parts.size() == 0)
            return;
        if (_parts[_parts.size() - 1].compare("#") == 0)
        {
            _parts.pop_back();
            _catch_all = true;
        }
    }

    bool uri_pattern::match(const std::vector<std::string>& parts) const
    {
        // topic must have equal or more parts that matcher
        if (_parts.size() > parts.size())
            return false;

        //if not ending in # we must have equal number of parts
        if (!_catch_all && _parts.size() != parts.size())
            return false;

        // a/b/#  vs a/b  -> false a/# vs a/b -> true
        if (_catch_all && (_parts.size() >= parts.size()))
            return false;

        //either this a string ending with open wild card or this is a string with equal nr of components
        //both ways we need to compare the parts items - if they are equal then  we match
        size_t n = _parts.size();
        for (size_t i = 0; i < n; ++i)
        {
            if (_parts[i].compare("+") && _parts[i].compare(parts[i]))
                return false;
        }

        assert(_parts.size() == parts.size() || _catch_all);

        return true;
    }

    /*
    bool uri_pattern::equal(const url_pattern& a) const
    {
    if (_catch_all != a._catch_all)
    return false;

    if (_parts.size() != a._parts.size())
    return false;

    size_t n = _parts.size();
    for (size_t i = 0; i < n; ++i)
    if (_parts[i].compare(a._parts[i]))
    return false;
    return true;
    }
    */
};