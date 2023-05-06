cmake_minimum_required (VERSION 3.8)

message ("-- Configuring extensions")

install (FILES on-modify.timewarrior totals.py DESTINATION ${TIMEW_DOCDIR}/ext)
