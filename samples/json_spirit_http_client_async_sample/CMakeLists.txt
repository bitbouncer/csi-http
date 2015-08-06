project(json_spirit_http_client_async_sample)

SET(SRCS
json_spirit_http_client_async_sample.cpp
)

add_executable(json_spirit_http_client_async_sample ${SRCS})
target_link_libraries(json_spirit_http_client_async_sample json_spirit ${EXT_LIBS})
