cmake_minimum_required (VERSION 2.8)

message ("-- Configuring man pages")

configure_file (man/timew.1.in   man/timew.1)
configure_file (man/timewrc.5.in man/timewrc.5)

install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/ DESTINATION ${TIMEW_MAN1DIR} FILES_MATCHING PATTERN "*.1")
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/ DESTINATION ${TIMEW_MAN5DIR} FILES_MATCHING PATTERN "*.5")

