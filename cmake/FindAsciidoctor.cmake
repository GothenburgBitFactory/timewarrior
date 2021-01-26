message (CHECK_START "Detecting Asciidoctor")

FIND_PROGRAM(ASCIIDOCTOR_EXECUTABLE asciidoctor
             PATHS "/usr/bin" "/usr/sbin")

MARK_AS_ADVANCED(ASCIIDOCTOR_EXECUTABLE)

if (ASCIIDOCTOR_EXECUTABLE)
  message(CHECK_PASS "found")
  message(DEBUG "Found executable ${ASCIIDOCTOR_EXECUTABLE}")
  set(ASCIIDOCTOR_FOUND "YES")
else (ASCIIDOCTOR_EXECUTABLE)
  message(CHECK_FAIL "not found")
  message(NOTICE "   Could not find Asciidoctor!")
  set(ASCIIDOCTOR_FOUND "NO")
endif (ASCIIDOCTOR_EXECUTABLE)
