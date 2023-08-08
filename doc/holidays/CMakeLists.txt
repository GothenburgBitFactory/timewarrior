cmake_minimum_required (VERSION 3.8)

message ("-- Configuring holiday documentation")

install (FILES README         DESTINATION ${TIMEW_DOCDIR}/holidays)
install (FILES holidays.en-US DESTINATION ${TIMEW_DOCDIR}/holidays)
install (FILES refresh        DESTINATION ${TIMEW_DOCDIR}/holidays)
