project(csi_http_server_sample)

SET(SRCS
csi_http_server_sample.cpp
)

add_executable(csi_http_server_sample ${SRCS})
target_link_libraries(csi_http_server_sample csi-json-spirit ${EXT_LIBS})
