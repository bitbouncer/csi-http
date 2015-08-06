SET(LIB_SRCS
	csi_http.h
	csi_http.cpp
	spinlock.cpp
	spinlock.h
	client/http_client.h
	client/http_client.cpp
	server/connection.h
	server/connection.cpp
	server/header.h
	server/reply.h
	server/reply.cpp
	server/request.h
    server/request.cpp
	server/server.h
	server/http_server.h
	server/http_server.cpp		
	server/http_connection.h			
	server/http_connection.cpp				
	utils/uri_pattern.h
    utils/uri_pattern.cpp
	encoding/avro_json_spirit_encoding.h
    encoding/avro_json_encoding.h
    encoding/avro_raw_binary_encoding.h
    encoding/http_rest_avro_json_encoding.h
    encoding/http_rest_avro_binary_encoding.h
    )
	
if (__CSI_HAS_OPENSSL__)
	SET(LIB_SRCS 
		${LIB_SRCS} 
		server/https_server.h
		server/https_server.cpp				
		server/https_connection.h
		server/https_connection.cpp					
	)
endif()

	
add_library (csi-http STATIC ${LIB_SRCS})

