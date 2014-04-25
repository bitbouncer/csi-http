project(avro_http_client_load_sample)

SET(SRCS
avro_http_client_load_sample.cpp
)

add_executable(avro_http_client_load_sample ${SRCS})
target_link_libraries(avro_http_client_load_sample ${EXT_LIBS})
