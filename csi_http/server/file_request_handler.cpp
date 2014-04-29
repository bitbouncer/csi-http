//
// file_request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "file_request_handler.h"
#include "reply.h"

namespace csi
{
    namespace http
    {

        static struct mapping
        {
            const char* extension;
            const char* mime_type;
        } mappings[] =
        {
            { "gif", "image/gif" },
            { "htm", "text/html" },
            { "html", "text/html" },
            { "jpg", "image/jpeg" },
            { "png", "image/png" },
            { "bin", "application/octet-stream" },
            { 0, 0 } // Marks end of list.
        };

        std::string extension_to_type(const std::string& extension)
        {
            for (mapping* m = mappings; m->extension; ++m)
            {
                if (m->extension == extension)
                {
                    return m->mime_type;
                }
            }

            return "text/plain";
        }

        file_request_handler::file_request_handler(const boost::filesystem::path& http_root) : _http_root(http_root)
        {
        }

        void file_request_handler::handle_request(const std::string& rel_url, csi::http::connection* context)
        {
            if (context->request().method() != csi::http::GET)
            {
                context->reply().create(csi::http::bad_request);
                return;
            }

            // must not not contain ".."
            if (rel_url.find("..") != std::string::npos)
            {
                context->reply().create(csi::http::bad_request);
                return;
            }

            boost::filesystem::path full_path(_http_root);
            full_path /= rel_url;

            // If path ends in slash (i.e. is a directory) then add "index.html". // we could autogen this file here....
            if (rel_url[rel_url.size() - 1] == '/')
            {
                full_path /= "index.html";
            }

            if (!boost::filesystem::exists(full_path))
            {
                context->reply().create(csi::http::not_found);
                return;
            }

            std::string extension = full_path.extension().generic_string();

            try
            {
                assert(false); // NOT implemneted for avro yet
                boost::uintmax_t file_size = boost::filesystem::file_size(full_path);
                //context->reply.contentX.reserve((size_t)file_size);
                boost::filesystem::ifstream ifs(full_path, std::ios_base::binary);
                //ifs.read(context->reply.contentX.data(), (std::streamsize) file_size);
                //context->reply.contentX.set_size((size_t)file_size);
            }
            catch (...)
            {
                context->reply().create(csi::http::not_found);
                return;
            }
            context->reply().create(csi::http::ok, extension_to_type(extension));
        }
    } // namespace server
} // namespace http
