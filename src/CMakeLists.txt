cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${TIMEW_INCLUDE_DIRS})

set (timew_SRCS Grammar.cpp     Grammar.h
                Lexer.cpp       Lexer.h
                LR0.cpp         LR0.h
                Rules.cpp       Rules.h)

add_library (timew STATIC ${timew_SRCS})
include_directories(${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)
add_executable (timew_executable timew.cpp)
add_executable (gr_executable gr.cpp)

target_link_libraries (timew_executable timew common ${TIMEW_LIBRARIES})
target_link_libraries (gr_executable timew common ${TIMEW_LIBRARIES})

set_property (TARGET timew_executable PROPERTY OUTPUT_NAME "timew")
set_property (TARGET gr_executable PROPERTY OUTPUT_NAME "gr")

install (TARGETS timew_executable DESTINATION bin)

