include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${PROTOBUF_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/backend
  ${CMAKE_SOURCE_DIR}/backend
  ${CMAKE_BINARY_DIR}/common
  ${CMAKE_SOURCE_DIR}/common
)

add_definitions(${QT_DEFINITIONS})
include(${QT_USE_FILE})

set(SOURCES
  main.cpp
  mainwindow.cpp
)

set(HEADERS
  mainwindow.h
)

set(UI
  mainwindow.ui
)

qt4_wrap_cpp(MOC ${HEADERS})
qt4_wrap_ui(UIC ${UI})

add_executable(trochus
  ${SOURCES}
  ${MOC}
  ${UIC}
)

target_link_libraries(trochus
  common
  backend
  ${QT_LIBRARIES}
  ${PROTOBUF_LITE_LIBRARY}
)

