include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROTOBUF_INCLUDE_DIRS}
  ${KERNELHEADERS_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/common
  ${CMAKE_SOURCE_DIR}/common
)

add_definitions(-DWITHOUT_QT)

set(SOURCES
  shell.cpp
  supervisor.cpp
  supervisormanager.cpp
)

add_library(backend STATIC
  ${SOURCES}
)

target_link_libraries(backend
  common
)

