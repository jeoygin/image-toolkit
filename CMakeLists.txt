cmake_minimum_required(VERSION 2.8)

option(LOCAL_INSTALL_DIR "Path to local install dir")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)
if(EXISTS ${LOCAL_INSTALL_DIR}/include)
  include_directories(${LOCAL_INSTALL_DIR}/include)
endif()
include_directories(/usr/include/opencv)
include_directories(/usr/include/opencv2)
include_directories(/usr/include)
include_directories(/usr/local/include)

if(EXISTS ${LOCAL_INSTALL_DIR}/lib)
  link_directories(${LOCAL_INSTALL_DIR}/lib)
endif()
link_directories(/usr/local/lib)

find_library(BOOST_SYSTEM_LIB NAMES libboost_system.a boost_system HINTS ${LOCAL_INSTALL_DIR}/lib)
find_library(BOOST_FILESYSTEM_LIB libboost_filesystem.a boost_filesystem HINTS ${LOCAL_INSTALL_DIR}/lib)
find_library(BOOST_PROGRAM_OPTIONS_LIB libboost_program_options.a boost_program_options HINTS ${LOCAL_INSTALL_DIR}/lib)
set(Boost_LIBRARIES ${BOOST_SYSTEM_LIB} ${BOOST_FILESYSTEM_LIB} ${BOOST_PROGRAM_OPTIONS_LIB})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/base64)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/fs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/db)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/cmd)

add_executable( imgtk image_toolkit.cpp )

target_link_libraries( imgtk cmd )
target_link_libraries( imgtk ${Boost_LIBRARIES} )
