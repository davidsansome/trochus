cmake_minimum_required(VERSION 2.6)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Find Qt
find_package(Qt4 4.5.0 REQUIRED QtCore QtGui QtNetwork)

# Find Protobuf
find_package(Protobuf)

find_library(PROTOBUF_LITE_LIBRARY protobuf-lite)
if(NOT PROTOBUF_LITE_LIBRARY)
  # Lucid doesn't have a .so symlink
  find_file(PROTOBUF_LITE_LIBRARY
    NAMES libprotobuf-lite.so.5
    PATH_SUFFIXES lib
  )
endif(NOT PROTOBUF_LITE_LIBRARY)

# Find threads
find_package(Threads)

# Find kernel headers
find_package(KernelHeaders REQUIRED)

# Subdirectories
add_subdirectory(common)
add_subdirectory(backend)
add_subdirectory(frontend)
