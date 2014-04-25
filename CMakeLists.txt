cmake_minimum_required(VERSION 2.8)

if(WIN32)
SET(OPENSSLPATH				${CMAKE_SOURCE_DIR}/../openssl-1.0.1e)
SET(ZLIB_PATH				${CMAKE_SOURCE_DIR}/../zlib-1.2.8)
SET(BOOST_PATH 		  		${CMAKE_SOURCE_DIR}/../boost_1_55_0)
SET(LIBCURL_PATH 	        ${CMAKE_SOURCE_DIR}/../curl-7.35.0)
SET(AVROCPP_PATH 	        ${CMAKE_SOURCE_DIR}/../avro-cpp-1.7.6)
endif()

if(__LINUX__)
SET(BOOST_PATH 		  		${CMAKE_SOURCE_DIR}/../boost_1_55_0)
SET(LIBCURL_PATH 	        ${CMAKE_SOURCE_DIR}/../curl-7.35.0)
SET(AVROCPP_PATH 	        ${CMAKE_SOURCE_DIR}/../avro-cpp-1.7.6)
endif()

if(__DARWIN__)
SET(BOOST_PATH 		  		${CMAKE_SOURCE_DIR}/../boost_1_55_0)
SET(LIBCURL_PATH 	        ${CMAKE_SOURCE_DIR}/../curl-7.35.0)
endif()

SET(LIBRARY_OUTPUT_PATH     ${CMAKE_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin)

add_definitions(-D_FILE_OFFSET_BITS=64 -D_REENTRANT -D EXTERNAL_LOCKS -DMULTITHREAD)
add_definitions(-DCURL_STATICLIB)
add_definitions(-DBOOST_ASIO_HAS_STD_CHRONO)

include_directories(${AVROCPP_PATH})

if(WIN32)
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

set(BOOST_LIB_PATH 		${BOOST_PATH}/lib/$(Platform)/lib)
set(CURL_LIB_PATH       ${LIBCURL_PATH}/libs/$(Platform)/$(Configuration))
set(CRYPTOPP_LIB_PATH 	${CRYPTOPP_PATH}/lib/$(Platform)/$(Configuration))
set(AVROCPP_LIB_PATH 	${AVROCPP_PATH}/lib/$(Platform)/$(Configuration))

SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/$(Platform))

include_directories(${OPENSSLPATH}/inc32)
link_directories(${CMAKE_SOURCE_DIR}/lib/$(Platform) ${OPENSSLPATH}/out32dll ${BOOST_LIB_PATH} ${CURL_LIB_PATH} ${CRYPTOPP_LIB_PATH} ${AVROCPP_LIB_PATH})
set(EXT_LIBS csi-http avrocpp_s libcurl libeay32 ssleay32 Ws2_32) 
#ZLIB 
endif()

if(__LINUX__)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
ADD_LIBRARY(LIBCURL STATIC IMPORTED)
SET_TARGET_PROPERTIES(LIBCURL PROPERTIES IMPORTED_LOCATION ${LIBCURL_PATH}/lib/.libs/libcurl.a)
ADD_LIBRARY(LIBCRYPTOPP STATIC IMPORTED)
SET_TARGET_PROPERTIES(LIBCRYPTOPP PROPERTIES IMPORTED_LOCATION ${CRYPTOPP_PATH}/lib/libcryptopp.a)
add_definitions(-DCURL_STATICLIB)
add_definitions(-D__LINUX__)
add_definitions(-fPIC)
add_definitions(-std=c++0x)
add_definitions(-DBOOST_LOG_DYN_LINK)
#add_definitions(-DBOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
add_definitions(-Wno-deprecated)

set(CRYPTOPP_LIB_PATH 	${CRYPTOPP_PATH}/lib)
set(AVROCPP_LIB_PATH 	${AVROCPP_PATH}/lib)

link_directories(${BOOST_PATH}/stage/lib ${CMAKE_SOURCE_DIR}/lib ${CURL_LIB_PATH})
set(EXT_LIBS avrocpp boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_program_options boost_filesystem boost_regex boost_chrono boost_locale LIBCURL LIBCRYPTOPP rt pthread c)
endif()

if(__DARWIN__)
SET (LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(TOBII_USDK_C_LIB_PATH ${TOBII_USDK_C_PATH}/lib)
set(TOBII_USDK_UTILS_LIB_PATH ${TOBII_UTILS_PATH}/lib)
ADD_LIBRARY(LIBCURL STATIC IMPORTED)
SET_TARGET_PROPERTIES(LIBCURL PROPERTIES IMPORTED_LOCATION ${LIBCURL_PATH}/lib/.libs/libcurl.a)
ADD_LIBRARY(LIBCRYPTOPP STATIC IMPORTED)
SET_TARGET_PROPERTIES(LIBCRYPTOPP PROPERTIES IMPORTED_LOCATION ${CRYPTOPP_PATH}/lib/libcryptopp.a)
add_definitions(-DCURL_STATICLIB)
add_definitions(-D__DARWIN__)
add_definitions(-fPIC)
add_definitions(-std=c++11)
add_definitions(-DBOOST_LOG_DYN_LINK)
include_directories(/opt/X11/include)
link_directories(${BOOST_PATH}/stage/lib ${CMAKE_SOURCE_DIR}/lib ${CURL_LIB_PATH})
set(EXT_LIBS boost_log_setup boost_log boost_date_time boost_timer boost_thread boost_system boost_filesystem boost_locale LIBCURL LIBCRYPTOPP stdc++ pthread c)
endif()
include_directories (
	${BOOST_PATH} 
	${CMAKE_SOURCE_DIR}
	${LIBCURL_PATH}/include
)

add_subdirectory(csi_http)
add_subdirectory(samples)
