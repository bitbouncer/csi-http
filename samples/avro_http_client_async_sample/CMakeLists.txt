project(avro_http_client_async_sample)

SET(SRCS
avro_http_client_async_sample.cpp
)

add_executable(avro_http_client_async_sample ${SRCS})
target_link_libraries(avro_http_client_async_sample ${EXT_LIBS})
