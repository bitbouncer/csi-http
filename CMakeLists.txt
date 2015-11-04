cmake_minimum_required(VERSION 2.8)
INCLUDE(${CMAKE_SOURCE_DIR}/../csi-build-scripts/CMakeDependencies.txt)

set(EXT_LIBS ${CSI_HTTP_LIBS})

#compile a http parser library from third party source
ADD_LIBRARY(csi-http-parser STATIC ${HTTP_PARSER_PATH}/http_parser.h ${HTTP_PARSER_PATH}/http_parser.c)

#compile a avro library from third party source since everything boost in avro build seems to conflict with our version
include_directories (${AVROCPP_PATH}/api)

ADD_LIBRARY(csi-avro STATIC 
        ${AVROCPP_PATH}/impl/Compiler.cc 
        ${AVROCPP_PATH}/impl/Node.cc
        ${AVROCPP_PATH}/impl/NodeImpl.cc 
        ${AVROCPP_PATH}/impl/ResolverSchema.cc 
        ${AVROCPP_PATH}/impl/Schema.cc
        ${AVROCPP_PATH}/impl/Types.cc 
        ${AVROCPP_PATH}/impl/ValidSchema.cc 
        ${AVROCPP_PATH}/impl/Zigzag.cc
        ${AVROCPP_PATH}/impl/BinaryEncoder.cc 
        ${AVROCPP_PATH}/impl/BinaryDecoder.cc
        ${AVROCPP_PATH}/impl/Stream.cc 
        ${AVROCPP_PATH}/impl/FileStream.cc
        ${AVROCPP_PATH}/impl/Generic.cc 
        ${AVROCPP_PATH}/impl/GenericDatum.cc
        ${AVROCPP_PATH}/impl/DataFile.cc
        ${AVROCPP_PATH}/impl/parsing/Symbol.cc
        ${AVROCPP_PATH}/impl/parsing/ValidatingCodec.cc
        ${AVROCPP_PATH}/impl/parsing/JsonCodec.cc
        ${AVROCPP_PATH}/impl/parsing/ResolvingDecoder.cc
        ${AVROCPP_PATH}/impl/json/JsonIO.cc
        ${AVROCPP_PATH}/impl/json/JsonDom.cc
        ${AVROCPP_PATH}/impl/Resolver.cc 
        ${AVROCPP_PATH}/impl/Validator.cc)

add_subdirectory(csi_http)
add_subdirectory(samples)
