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
	#server/http_parser.h
	#server/http_parser.cpp
	server/header.h
	server/io_service_pool.h
	server/io_service_pool.cpp
	server/reply.h
	server/reply.cpp
	server/request.h
	server/request_handler.h
	server/server.h
	server/server.cpp
	server/unsecure_server.h
	server/unsecure_server.cpp		
	server/unsecure_connection.h			
	server/unsecure_connection.cpp				
	)
	
if (__CSI_HAS_OPENSSL__)
	SET(LIB_SRCS 
		${LIB_SRCS} 
		server/ssl_server.h
		server/ssl_server.cpp				
		server/ssl_connection.h
		server/ssl_connection.cpp					
	)
endif()

	
add_library (csi-http STATIC ${LIB_SRCS})

