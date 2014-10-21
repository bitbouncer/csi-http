cmake_minimum_required(VERSION 2.8)

SET(BOOST_PATH          ${CMAKE_SOURCE_DIR}/../boost_1_55_0)
SET(AVROCPP_PATH        ${CMAKE_SOURCE_DIR}/../avro-cpp-1.7.7)
SET(OPENSSLPATH         ${CMAKE_SOURCE_DIR}/../openssl-1.0.1g)
SET(LIBCURL_PATH        ${CMAKE_SOURCE_DIR}/../curl-7.35.0)
SET(HTTP_PARSER_PATH    ${CMAKE_SOURCE_DIR}/../http-parser-2.3)
SET(JSON_SPIRIT_PATH    ${CMAKE_SOURCE_DIR}/../json-spirit)

SET(LIBRARY_OUTPUT_PATH     ${CMAKE_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin)

add_definitions(-D_FILE_OFFSET_BITS=64 -D_REENTRANT -D EXTERNAL_LOCKS -DMULTITHREAD)
add_definitions(-DBOOST_ASIO_HAS_STD_CHRONO)

include_directories (
    ${BOOST_PATH} 
    ${CMAKE_SOURCE_DIR}
    ${LIBCURL_PATH}/include
    ${AVROCPP_PATH}
	${HTTP_PARSER_PATH}
	${JSON_SPIRIT_PATH})


if(WIN32)
add_definitions(-DCURL_STATICLIB)
ADD_LIBRARY(ZLIB STATIC IMPORTED)
SET_TARGET_PROPERTIES(ZLIB PROPERTIES IMPORTED_LOCATION ${ZLIB_PATH}/lib/zdll.lib)

add_definitions(-D_WIN32_WINNT=0x0501)
add_definitions(-DCF_WINDOWS)
add_definitions(/MP)
add_definitions(/bigobj)
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(/wd4996) #'strdup': The POSIX name for this item is deprecated
add_definitions(/wd4197) # 'cf_atomic32' : top-level volatile in cast is ignored

set(BOOST_LIB_PATH       ${BOOST_PATH}/lib/$(Platform)/lib)
set(CURL_LIB_PATH        ${LIBCURL_PATH}/libs/$(Platform)/$(Configuration))
set(AVROCPP_LIB_PATH     ${AVROCPP_PATH}/lib/$(Platform)/$(Configuration))
set(JSON_SPIRIT_LIB_PATH ${JSON_SPIRIT_PATH}/lib/$(Platform)/$(Configuration))

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/$(Platform))

include_directories(${OPENSSLPATH}/inc32)
link_directories(${CMAKE_SOURCE_DIR}/lib/$(Platform) ${OPENSSLPATH}/out32dll ${BOOST_LIB_PATH} ${CURL_LIB_PATH} ${AVROCPP_LIB_PATH} ${JSON_SPIRIT_LIB_PATH})
set(EXT_LIBS csi-http csi-http-parser avrocpp_s libcurl libeay32 ssleay32 Ws2_32) 
endif()

if(__LINUX__)
set(AVROCPP_LIB_PATH ${AVROCPP_PATH}/build)
set(CURL_LIB_PATH  ${LIBCURL_PATH}/lib/.libs)
set(JSON_SPIRIT_LIB_PATH ${JSON_SPIRIT_PATH}/lib)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

add_definitions(-D__LINUX__)
add_definitions(-fPIC)
add_definitions(-std=c++11)
add_definitions(-DBOOST_LOG_DYN_LINK)
#add_definitions(-DBOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
add_definitions(-Wno-deprecated)
include_directories(${OPENSSLPATH}/include)
link_directories(${BOOST_PATH}/stage/lib ${CMAKE_SOURCE_DIR}/lib ${AVROCPP_LIB_PATH} ${CURL_LIB_PATH} ${JSON_SPIRIT_LIB_PATH} ${OPENSSLPATH})
set(EXT_LIBS csi-http csi-http-parser avrocpp_s boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_program_options boost_filesystem boost_regex boost_chrono curl ssl crypto rt pthread c)
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
set(EXT_LIBS csi-http csi-http-parser avrocpp_s boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_filesystem boost_locale LIBCURL crypto ssl stdc++ pthread c)
endif()

#compile a library from third party source
ADD_LIBRARY(csi-http-parser STATIC ${HTTP_PARSER_PATH}/http_parser.h ${HTTP_PARSER_PATH}/http_parser.c)

add_subdirectory(csi_http)
add_subdirectory(samples)
