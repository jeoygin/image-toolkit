add_library(cmd
  command_handler.cpp
  operation.cpp
  op_binary.cpp
  op_cat.cpp
  op_clip.cpp
  op_equ.cpp
  op_gray.cpp
  op_info.cpp
  op_invert.cpp
  op_resize.cpp
  op_rotate.cpp
  op_save.cpp
  op_show.cpp
  op_sizeup.cpp
  op_template.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")

find_library(OPENCV_IMGCODECS_LIB opencv_imgcodecs)

target_link_libraries(cmd base64)
target_link_libraries(cmd db)

target_link_libraries(cmd boost_system)
target_link_libraries(cmd boost_filesystem)
target_link_libraries(cmd boost_program_options)
target_link_libraries(cmd glog)
target_link_libraries(cmd opencv_core)
target_link_libraries(cmd opencv_highgui)
target_link_libraries(cmd opencv_imgproc)
target_link_libraries(cmd yaml-cpp)

if(OPENCV_IMGCODECS_LIB)
  target_link_libraries(cmd opencv_imgcodecs)
endif()
