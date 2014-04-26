cmake_minimum_required(VERSION 2.8)


SET(BOOST_PATH          ${CMAKE_SOURCE_DIR}/../boost_1_55_0)
SET(AVROCPP_PATH        ${CMAKE_SOURCE_DIR}/../avro-cpp-1.7.6)
SET(OPENSSLPATH         ${CMAKE_SOURCE_DIR}/../openssl-1.0.1g)
SET(LIBCURL_PATH        ${CMAKE_SOURCE_DIR}/../curl-7.35.0)

SET(LIBRARY_OUTPUT_PATH     ${CMAKE_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin)

add_definitions(-D_FILE_OFFSET_BITS=64 -D_REENTRANT -D EXTERNAL_LOCKS -DMULTITHREAD)
add_definitions(-DBOOST_ASIO_HAS_STD_CHRONO)

include_directories (
    ${BOOST_PATH} 
    ${CMAKE_SOURCE_DIR}
    ${LIBCURL_PATH}/include
    ${AVROCPP_PATH})

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

set(BOOST_LIB_PATH      ${BOOST_PATH}/lib/$(Platform)/lib)
set(CURL_LIB_PATH       ${LIBCURL_PATH}/libs/$(Platform)/$(Configuration))
set(CRYPTOPP_LIB_PATH   ${CRYPTOPP_PATH}/lib/$(Platform)/$(Configuration))
set(AVROCPP_LIB_PATH    ${AVROCPP_PATH}/lib/$(Platform)/$(Configuration))

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/$(Platform))

include_directories(${OPENSSLPATH}/inc32)
link_directories(
    ${CMAKE_SOURCE_DIR}/lib/$(Platform) 
    ${OPENSSLPATH}/out32dll 
    ${BOOST_LIB_PATH} 
    ${CURL_LIB_PATH} 
    ${AVROCPP_LIB_PATH})
set(EXT_LIBS csi-http avrocpp_s libcurl libeay32 ssleay32 Ws2_32) 
endif()

if(__LINUX__)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
add_definitions(-D__LINUX__)
add_definitions(-fPIC)
add_definitions(-std=c++11)
add_definitions(-DBOOST_LOG_DYN_LINK)
#add_definitions(-DBOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
add_definitions(-Wno-deprecated)
set(AVROCPP_LIB_PATH ${AVROCPP_PATH}/build)
link_directories(${BOOST_PATH}/stage/lib ${CMAKE_SOURCE_DIR}/lib ${AVROCPP_LIB_PATH})
set(EXT_LIBS csi-http avrocpp_s boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_program_options boost_filesystem boost_regex boost_chrono boost_locale curl rt pthread c)
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
link_directories(${BOOST_PATH}/stage/lib ${CMAKE_SOURCE_DIR}/lib ${CURL_LIB_PATH} ${AVROCPP_LIB_PATH})
set(EXT_LIBS csi-http avrocpp_s boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_filesystem boost_locale LIBCURL LIBCRYPTOPP stdc++ pthread c)
endif()


add_subdirectory(csi_http)
add_subdirectory(samples)
