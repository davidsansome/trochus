include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROTOBUF_INCLUDE_DIRS}
)

add_definitions(-DWITHOUT_QT)

set(SOURCES
  logging.cpp
  messagingobject.cpp
  messagingprocess.cpp
)

set(MESSAGES
  messages.proto
)

protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS ${MESSAGES})

add_library(common STATIC
  ${SOURCES}
  ${PROTO_SOURCES}
)

target_link_libraries(common
  ${PROTOBUF_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
)

