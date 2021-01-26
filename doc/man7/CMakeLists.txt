cmake_minimum_required (VERSION 2.8.12)

file (GLOB DOC_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/*.7.adoc")
set (DOC_FILES)

foreach (SRC ${DOC_SOURCES})
  string (REPLACE ".adoc" "" OUTPUT_FILE_NAME "${SRC}")

  add_custom_command (
          OUTPUT "${OUTPUT_FILE_NAME}"
          COMMAND ${ASCIIDOCTOR_EXECUTABLE} -b manpage ${ASCIIDOCTOR_OPTIONS} ${SRC} -o ${OUTPUT_FILE_NAME}
          DEPENDS "${SRC}")

  set (DOC_FILES ${DOC_FILES} "${OUTPUT_FILE_NAME}")
endforeach (SRC)

add_custom_target (man7 DEPENDS ${DOC_FILES})

install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         DESTINATION ${TIMEW_MANDIR}
         FILES_MATCHING PATTERN "*.7")
