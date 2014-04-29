project(avro_https_client_sample)

SET(SRCS
avro_https_client_sample.cpp
)

add_executable(avro_https_client_sample ${SRCS})
target_link_libraries(avro_https_client_sample ${EXT_LIBS})
