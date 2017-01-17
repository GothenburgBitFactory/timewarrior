cmake_minimum_required (VERSION 2.8)

message ("-- Configuring theme documentation")

install (FILES README           DESTINATION ${TIMEW_DOCDIR}/doc/themes)
INSTALL (FILES dark.theme       DESTINATION ${TIMEW_DOCDIR}/doc/themes)
INSTALL (FILES dark_blue.theme  DESTINATION ${TIMEW_DOCDIR}/doc/themes)
INSTALL (FILES dark_green.theme DESTINATION ${TIMEW_DOCDIR}/doc/themes)
INSTALL (FILES dark_red.theme   DESTINATION ${TIMEW_DOCDIR}/doc/themes)
