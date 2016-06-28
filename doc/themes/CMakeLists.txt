cmake_minimum_required (VERSION 2.8)

message ("-- Configuring theme documentation")

install (FILES README     DESTINATION ${TIMEW_DOCDIR}/doc/themes)
install (FILES dark.theme DESTINATION ${TIMEW_DOCDIR}/doc/themes)
