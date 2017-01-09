set(SRC encoder.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")

add_library(encode ${SRC})

target_link_libraries(encode base64)
