project(csi_http_client_async_sample)

SET(SRCS
csi_http_client_async_sample.cpp
)

add_executable(csi_http_client_async_sample ${SRCS})
target_link_libraries(csi_http_client_async_sample csi-json-spirit ${EXT_LIBS})
