project(hello_google_sample)

SET(SRCS
hello_google_sample.cpp
)

add_executable(hello_google_sample ${SRCS})
target_link_libraries(hello_google_sample ${EXT_LIBS})
