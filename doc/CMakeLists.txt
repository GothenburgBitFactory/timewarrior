cmake_minimum_required (VERSION 2.8)

message ("-- Configuring documentation")

add_subdirectory (holidays)
add_subdirectory (themes)

configure_file (man/timew.1.in man/timew.1)

install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/ DESTINATION ${TIMEW_MAN1DIR} FILES_MATCHING PATTERN "*.1")
