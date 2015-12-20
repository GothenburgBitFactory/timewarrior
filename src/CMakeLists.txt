cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/common
                     ${TIMEW_INCLUDE_DIRS})

set (timew_SRCS Grammar.cpp     Grammar.h
                Lexer.cpp       Lexer.h
                LR0.cpp         LR0.h)

add_library (timew STATIC ${timew_SRCS})
add_executable (timew_executable timew.cpp)

target_link_libraries (timew_executable common timew ${TIMEW_LIBRARIES})

set_property (TARGET timew_executable PROPERTY OUTPUT_NAME "timew")

install (TARGETS timew_executable DESTINATION ${TIMEW_BINDIR})

