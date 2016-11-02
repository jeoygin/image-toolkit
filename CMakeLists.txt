cmake_minimum_required(VERSION 2.8)

option(LOCAL_INSTALL_DIR "Path to local install dir")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/usr/include/opencv)
include_directories(/usr/include/opencv2)
include_directories(/usr/include)
include_directories(/usr/local/include)

if(EXISTS ${LOCAL_INSTALL_DIR}/lib)
  link_directories(${LOCAL_INSTALL_DIR}/lib)
endif()
link_directories(/usr/local/lib)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/base64)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/fs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/db)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/cmd)

add_executable( imgtk image_toolkit.cpp )

target_link_libraries( imgtk cmd )
