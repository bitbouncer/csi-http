//
// request_handler.h
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

#include "reply.h"
#include "header.h"
#include "connection.h"

namespace csi
{
    namespace http_server
    {
        class connection;

        /// The common handler for all incoming requests.
        class request_handler : private boost::noncopyable
        {
        public:
            request_handler() : _nr_of_requests(0) {}
            /// Handle a request and produce a reply.
            virtual void handle_request(const std::string& rel_url, csi::http_server::connection*) = 0;
            void virtual incr_request_count()			      { csi::spinlock::scoped_lock xx(_spinlock); ++_nr_of_requests; }
            uint64_t virtual get_no_of_requests() const { return _nr_of_requests; }
        protected:
            csi::spinlock   _spinlock;
            uint64_t        _nr_of_requests;
        };
    } // namespace server
} // namespace csi
