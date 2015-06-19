cmake_minimum_required(VERSION 2.8)
INCLUDE(${CMAKE_SOURCE_DIR}/../csi-build-scripts/CMakeDependencies.txt)


if(WIN32)
set(EXT_LIBS csi-http csi-http-parser csi-avro avrocpp_s libcurl libeay32 ssleay32 Ws2_32) 
endif()

if(__LINUX__)
set(EXT_LIBS csi-http csi-http-parser csi-avro avrocpp_s boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_program_options boost_filesystem boost_regex boost_chrono curl cares ssl crypto rt pthread c)
endif()

if(__DARWIN__)
set(AVROCPP_LIB_PATH    ${AVROCPP_PATH}/lib)
SET (LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
ADD_LIBRARY(LIBCURL STATIC IMPORTED)
SET_TARGET_PROPERTIES(LIBCURL PROPERTIES IMPORTED_LOCATION ${LIBCURL_PATH}/lib/.libs/libcurl.a)
add_definitions(-DCURL_STATICLIB)
add_definitions(-D__DARWIN__)
add_definitions(-fPIC)
add_definitions(-std=c++11)
add_definitions(-DBOOST_LOG_DYN_LINK)
include_directories(/opt/X11/include)
link_directories(${BOOST_PATH}/stage/lib ${CMAKE_SOURCE_DIR}/lib ${CURL_LIB_PATH} ${AVROCPP_LIB_PATH} ${JSON_SPIRIT_LIB_PATH})
set(EXT_LIBS csi-http csi-http-parser csi-avro avrocpp_s boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_filesystem boost_locale LIBCURL crypto ssl stdc++ pthread c)
endif()

#compile a library from third party source
ADD_LIBRARY(csi-http-parser STATIC ${HTTP_PARSER_PATH}/http_parser.h ${HTTP_PARSER_PATH}/http_parser.c)

add_subdirectory(csi_http)
add_subdirectory(samples)
