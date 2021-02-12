cmake_minimum_required (VERSION 2.8.12)

message ("-- Configuring holiday documentation")

install (FILES README         DESTINATION ${TIMEW_DOCDIR}/doc/holidays)
install (FILES holidays.en-US DESTINATION ${TIMEW_DOCDIR}/doc/holidays)
install (FILES refresh        DESTINATION ${TIMEW_DOCDIR}/doc/holidays)
