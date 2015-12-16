#include <csi_http/client/http_client.h>
#include "avro_json_encoding.h"
#pragma once

namespace csi
{  
  template<class Request>
  std::shared_ptr<http_client::call_context> create_avro_json_rest(csi::http::method_t method, const std::string& uri, const Request& request, const std::vector<std::string>& headers, const std::chrono::milliseconds& timeout)
    {
        std::shared_ptr<http_client::call_context> p(new http_client::call_context(method, uri, headers, timeout));
        avro_json_encode(request, p->tx_content());
        return p;
    }
}