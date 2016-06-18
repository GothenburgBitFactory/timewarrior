cmake_minimum_required (VERSION 2.8)

message ("-- Configuring documentation")

configure_file (man/timew.1.in man/timew.1)

install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/             DESTINATION ${TIMEW_MAN1DIR} FILES_MATCHING PATTERN "*.1")
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/themes/          DESTINATION ${TIMEW_DOCDIR}/themes)
install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/holidays/        DESTINATION ${TIMEW_DOCDIR}/holidays)

