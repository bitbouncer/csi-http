project(avro_https_server_sample)

SET(SRCS
avro_https_server_sample.cpp

)

add_executable(avro_https_server_sample ${SRCS})
target_link_libraries(avro_https_server_sample ${EXT_LIBS})
