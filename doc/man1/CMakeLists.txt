cmake_minimum_required (VERSION 3.10)

if (ASCIIDOCTOR_FOUND)
  file (GLOB DOC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.1.adoc")
  set (DOC_FILES)

  foreach (SRC IN LISTS DOC_SOURCES)
    string (REPLACE ".adoc" "" OUTPUT_FILE_NAME "${SRC}")
    string (REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" OUTPUT_FILE_NAME "${OUTPUT_FILE_NAME}")

    add_custom_command (OUTPUT "${OUTPUT_FILE_NAME}"
                        COMMAND ${ASCIIDOCTOR_EXECUTABLE} -b manpage ${ASCIIDOCTOR_OPTIONS} "${SRC}" -o "${OUTPUT_FILE_NAME}"
                        DEPENDS "${SRC}")

    list (APPEND DOC_FILES "${OUTPUT_FILE_NAME}")
  endforeach (SRC)

  add_custom_target (man1 DEPENDS ${DOC_FILES})
else (ASCIIDOCTOR_FOUND)
  file (GLOB MAN_PAGES "${CMAKE_CURRENT_SOURCE_DIR}/*.1")
  set (DOC_FILES ${MAN_PAGES})
endif (ASCIIDOCTOR_FOUND)

install (FILES ${DOC_FILES} DESTINATION ${TIMEW_MAN1DIR})
