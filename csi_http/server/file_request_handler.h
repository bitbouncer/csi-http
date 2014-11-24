//
// file_request_handler.h
// ~~~~~~~~~~~~~~~~~~~
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
#include "request_handler.h"

namespace csi
{
    namespace http
    {
        class connection;
        class file_request_handler : public request_handler
        {
        public:
            file_request_handler(const boost::filesystem::path& http_root);
            virtual void handle_request(const std::string& rel_url, csi::http::connection*);
        private:
            boost::filesystem::path                        _http_root;
        };
    } // namespace server
} // namespace http
