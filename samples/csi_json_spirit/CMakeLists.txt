SET(JSON_SPIRIT_SRCS
json_spirit_reader.cpp 
json_spirit_writer.cpp 
json_spirit_value.cpp 
)

FILE (GLOB PUBLIC_HEADERS "*.h")
 
ADD_LIBRARY(csi-json-spirit STATIC ${JSON_SPIRIT_SRCS} ${PUBLIC_HEADERS})

