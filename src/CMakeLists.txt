cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${TIMEW_INCLUDE_DIRS})

set (timew_SRCS Color.h Color.cpp
                text.h  text.cpp
                utf8.h  utf8.cpp
                        wcwidth6.cpp)

add_library (timew STATIC ${timew_SRCS})
add_executable (timew_executable main.cpp)

target_link_libraries (timew_executable timew ${TIMEW_LIBRARIES})

set_property (TARGET timew_executable PROPERTY OUTPUT_NAME "timew")

install (TARGETS timew_executable DESTINATION ${TIMEW_BINDIR})

