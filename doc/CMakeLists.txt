cmake_minimum_required (VERSION 3.10)

message ("-- Configuring documentation")

add_subdirectory (holidays)
add_subdirectory (themes)

if (ASCIIDOCTOR_FOUND)
  set (ASCIIDOCTOR_OPTIONS "--attribute=manmanual=User Manuals"
                           "--attribute=mansource=timew ${PROJECT_VERSION}")
endif (ASCIIDOCTOR_FOUND)

add_subdirectory (man1)
add_subdirectory (man7)

add_custom_target (doc ALL DEPENDS man1 man7)
