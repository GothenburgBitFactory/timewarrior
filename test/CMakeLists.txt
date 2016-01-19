cmake_minimum_required (VERSION 2.8)
if(POLICY CMP0037)
  cmake_policy(SET CMP0037 OLD)
endif()

include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/test)

include_directories (${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

set (test_SRCS grammar.t lexer.t lr0.t pig.t rules.t)

add_custom_target (test ./run_all --verbose
                        DEPENDS ${test_SRCS}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test)

foreach (src_FILE ${test_SRCS})
  add_executable (${src_FILE} "${src_FILE}.cpp" test.cpp)
  target_link_libraries (${src_FILE} timew common ${TIMEW_LIBRARIES})
endforeach (src_FILE)

configure_file(run_all run_all COPYONLY)

