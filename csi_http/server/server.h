//
// server.h
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

//#include <map>
#include <vector>
#include <memory>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <csi_http/spinlock.h>
#include <csi_http/utils/uri_pattern.h>
#include <boost/algorithm/string.hpp>

/*
//        * The + wildcard may be used at any point within the subscription and is a
//        * wildcard for only the level of hierarchy at which it is placed.
//        * Neither wildcard may be used as part of a substring.
//        * Valid:
//        * 	a/b/+
//        * 	a/+/c
//        * 	a/#
//        * 	a/b/#
//        * 	#
//        * 	+/b/c
//        * 	+/+/+
//        * Invalid:
//        *	a/#/c
//        *	a+/b/c
//        * Valid but non-matching:
//        *	a/b
//        *	a/+
//        *	+/b
//        *	b/c/a
//        *	a/b/d
//        */



namespace csi
{
    namespace http
    {
        //class connection;

        template<class connection_type>
        class server : private boost::noncopyable
        {
        protected:
            server() {}
        public:
            typedef boost::function <void(const std::vector<std::string>&, std::shared_ptr<connection_type>)> handler;
            
            void add_handler(const std::string& pattern, handler p) 
            { 
                _handlers2.emplace_back(std::make_shared<handler_data>(pattern, p)); 
            }

            /// Handle a request and produce a reply.
            void handle_request(std::shared_ptr<connection_type> connection)
            {
                const std::string& url = connection->request().url();
                std::vector<std::string> parts;
                boost::split(parts, url, boost::is_any_of("/"));

                for (std::vector<std::shared_ptr<handler_data>>::const_iterator i = _handlers2.begin(); i != _handlers2.end(); ++i)
                {
                    if ((*i)->match(parts))
                    {
                        (*i)->call(parts, connection);
                        return;
                    }
                }
                connection->reply().create(csi::http::not_found);
            }

            uint64_t get_no_of_requests(const std::string& pattern = "#") const
            {
                std::vector<std::string> parts;
                boost::split(parts, pattern, boost::is_any_of("/"));

                uint64_t result = 0;
                for (std::vector<std::shared_ptr<handler_data>>::const_iterator i = _handlers2.begin(); i != _handlers2.end(); ++i)
                {
                    // now
                    if ((*i)->match(parts))
                        result += (*i)->get_no_of_requests();
                }
                return result;
            }


        private:

            // better name..
            class handler_data
            {
            public:
                handler_data(const std::string& pattern, handler h) : _pattern(pattern), _handler(h) {}

                inline bool match(const std::vector<std::string>& uri) const
                {
                    return _pattern.match(uri);
                }

                inline void call(const std::vector<std::string>& uri, std::shared_ptr<connection_type> c) const
                {
                    csi::spinlock::scoped_lock xx(_spinlock);
                    ++_nr_of_requests;
                    _handler(uri, c);
                }

                uint64_t get_no_of_requests() const { return _nr_of_requests; }

            protected:
                uri_pattern _pattern;
                handler     _handler;

                // more metrics
                mutable csi::spinlock   _spinlock;
                mutable uint64_t        _nr_of_requests;  // total_calls
                // calls_per_second
                // average_call_time;
                // 99% call time
            };

            std::vector<std::shared_ptr<handler_data>> _handlers2;
        };
    };
};
