add_library(fs
  fs.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")

target_link_libraries(fs base64)
