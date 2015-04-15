#include <string>
#include <vector>

#pragma once

namespace csi
{
    class uri_pattern
    {
    public:
        uri_pattern(const std::string&);
        bool match(const std::vector<std::string>& parts) const;
        //bool equal (const url_pattern&) const;
        const std::vector<std::string>& parts() const { return _parts; }
    private:
        std::vector<std::string> _parts;
        bool                     _catch_all; // # last
    };
}