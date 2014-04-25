//
// io_service_pool.h
// ~~~~~~~~~~~~~~~~~~~
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace csi
{
    namespace http_server
    {
        /// A pool of io_service objects.
        class io_service_pool : private boost::noncopyable
        {
        public:
            /// Construct the io_service pool.
            explicit io_service_pool(std::size_t pool_size);

            /// Run all io_service objects in the pool.
            void run();

            /// Stop all io_service objects in the pool.
            void stop();

            /// Get an io_service to use.
            boost::asio::io_service& get_io_service();

        private:
            typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
            typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

            /// The pool of io_services.
            std::vector<io_service_ptr> _io_services;

            /// The work that keeps the io_services running.
            std::vector<work_ptr>       _work;

            /// The next io_service to use for a connection.
            std::size_t                 _next_io_service;
        };

    } // namespace server
} // namespace http
