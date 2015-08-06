project(json_spirit_http_server_sample)

SET(SRCS
json_spirit_http_server_sample.cpp
)

add_executable(json_spirit_http_server_sample ${SRCS})
target_link_libraries(json_spirit_http_server_sample json_spirit ${EXT_LIBS})
