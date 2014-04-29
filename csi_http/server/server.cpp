//
// server.cpp
// ~~~~~~~~~
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.h"
#include "connection.h"
#include "request_handler.h"

namespace csi
{
    namespace http
    {
        server::server() { }

        void server::add_request_handler(const std::string& vpath, request_handler* p)
        {
            _handlers.insert(std::pair<std::string, request_handler*>(vpath, p));
        }

        /// Handle a request and produce a reply.
        void server::handle_request(csi::http::connection* connection)
        {
            const std::string& url = connection->request().url();
            size_t url_len = connection->request().url().size();

            for (std::map<std::string, request_handler*>::const_reverse_iterator i = _handlers.rbegin(); i != _handlers.rend(); ++i)
            {
                size_t vpath_len = (*i).first.size();
                if ((url_len >= vpath_len) && (url.compare(0, vpath_len, (*i).first) == 0))
                {
                    std::string sub_path = url.substr(vpath_len);
                    ((*i).second)->handle_request(sub_path, connection);
                    ((*i).second)->incr_request_count(); // possibly add the reply status here to make better statistics

                    if (!connection->waiting_for_reply())
                    {
                        // failures is always logged as warnings
                        /*
                        if (context->reply.status>=300)
                        {
                        LOG_WARNING() << to_string(context->auth._remote_endpoint) << " id: " << remote_id_to_string(context->auth) << " " << http_method_str(context->request_method()) << " " << context->request_url() << " http_res: " << context->reply.status << " content_length: " << context->reply.content.size();
                        }
                        else if (_logging_state)
                        {
                        LOG_INFO() << to_string(context->auth._remote_endpoint) << " id: " << remote_id_to_string(context->auth) << " " << http_method_str(context->request_method()) << " " << context->request_url() << " http_res: " << context->reply.status << " content_length: " << context->reply.content.size();
                        }
                        */
                    }
                    else
                    {
                        //LOG_DEBUG()  << to_string(context->auth._remote_endpoint) << " id: " << remote_id_to_string(context->auth) << " " << http_method_str(context->request_method()) << " " << context->request_url() << "ASYNC WAIT";
                    }
                    return;
                }
            }

            // if we failed to find a handler - we better do something with this connection...
            connection->reply().create(csi::http::not_found);
            //LOG_WARNING() << to_string(context->auth._remote_endpoint) << " id: " << remote_id_to_string(context->auth) << " " << http_method_str(context->request_method()) << " " << context->request_url() << " status: " << context->reply.status;
        }

    };
};