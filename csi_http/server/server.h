#pragma once

#include <map>
#include <boost/noncopyable.hpp>

namespace csi
{
    namespace http_server
    {
        class request_handler;
        class connection;

        class server : private boost::noncopyable
        {
        protected:
            server();

        public:
            void add_request_handler(const std::string& vpath, request_handler* p);
            void handle_request(csi::http_server::connection*);
        private:
            std::map<std::string, request_handler*> _handlers;
            static bool url_decode(const std::string& in, std::string& out);
        };
    };
};
