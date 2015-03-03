SET(LIB_SRCS
	csi_http.h
	csi_http.cpp
	spinlock.cpp
	spinlock.h
	avro_encoding.cpp
	avro_encoding.h
	json_encoding.h
	client/http_client.h
	client/http_client.cpp
	server/connection.h
	server/connection.cpp
	server/file_request_handler.cpp
	server/file_request_handler.h
	server/header.h
	server/reply.h
	server/reply.cpp
	server/request.h
	server/request_handler.h
	server/server.h
	server/server.cpp
	server/http_server.h
	server/http_server.cpp		
	server/http_connection.h			
	server/http_connection.cpp				
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

