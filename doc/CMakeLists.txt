cmake_minimum_required (VERSION 3.8)

message ("-- Configuring documentation")

add_subdirectory (holidays)
add_subdirectory (themes)

if (ASCIIDOCTOR_FOUND)
  set (ASCIIDOCTOR_OPTIONS "--attribute=manmanual=User Manuals"
                           "--attribute=mansource=timew ${PROJECT_VERSION}"
                           "--attribute=man-linkstyle='pass:[blue R < >]'")

  add_subdirectory (man1)
  add_subdirectory (man7)

  add_custom_target (doc ALL DEPENDS man1 man7)
else (ASCIIDOCTOR_FOUND)
  add_custom_target (doc ALL
                     COMMAND echo "Tool 'asciidoctor' is required to generate man pages! Please install it first.")
endif (ASCIIDOCTOR_FOUND)
