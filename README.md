csi-http
========

async http client and server library written in C++11, builtin support for apache avro
- syncronous  client based on libcurl and boost ASIO
- asyncronous client based on libcurl and boost ASIO
- asyncronous high performance server based on Joyents http_parser and boost ASIO
- generic GET, PUT, POST
- REST calls using avro binary and json encoded payloads (using POST)
- REST calls using "generic" json encoded payloads (with optional json-spirit library)
- HTTP 1.1, specifically supports connection:keep-alive
- support for HTTPS using OpenSSL
 
Missing:
- no support for avro rpc (REST calls with avro payload supported)

Platforms:
 - Windows, Visual studio 2013
 - Linux, GCC
 - Raspberry Pi, GCC


Building
see
https://github.com/bitbouncer/csi-build-scripts


License:
- Boost Software License, Version 1.0.











