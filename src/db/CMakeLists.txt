set(SRC db.cpp db_filedb.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -O2")

find_library(LEVELDB_LIB NAMES leveldb PATHS ${LOCAL_INSTALL_DIR}/lib)
if(LEVELDB_LIB)
  set(SRC ${SRC} db_leveldb.cpp)
  add_definitions(-DWITH_LEVELDB)
endif()

find_library(ROCKSDB_LIB NAMES rocksdb PATHS ${LOCAL_INSTALL_DIR}/lib)
if(ROCKSDB_LIB)
  set(SRC ${SRC} db_rocksdb.cpp)
  add_definitions(-DWITH_ROCKSDB)
endif()

find_library(LMDB_LIB NAMES lmdb PATHS ${LOCAL_INSTALL_DIR}/lib)
if(LMDB_LIB)
  set(SRC ${SRC} db_lmdb.cpp)
  add_definitions(-DWITH_LMDB)
endif()

add_library(db ${SRC})

target_link_libraries(db fs)
target_link_libraries(db base64)

target_link_libraries(db boost_system)
target_link_libraries(db glog)
target_link_libraries(db pthread)

if(LEVELDB_LIB)
  target_link_libraries(db leveldb)
endif()

if(LMDB_LIB)
  target_link_libraries(db lmdb)
endif()

if(ROCKSDB_LIB)
  target_link_libraries(db rocksdb)
  target_link_libraries(db z)
  target_link_libraries(db lz4)
  target_link_libraries(db bz2)
  target_link_libraries(db snappy)
endif()
