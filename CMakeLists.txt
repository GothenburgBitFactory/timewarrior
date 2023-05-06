cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set (HAVE_CMAKE true)

project (timew)
include (CXXSniffer)
include (FindAsciidoctor)

set (PROJECT_VERSION "1.5.0-dev")

string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

message ("-- Looking for SHA1 references")
if (EXISTS ${CMAKE_SOURCE_DIR}/.git/index)
  set (HAVE_COMMIT true)
  execute_process (COMMAND git log -1 --pretty=format:%h
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                   OUTPUT_VARIABLE COMMIT)
  configure_file (${CMAKE_SOURCE_DIR}/commit.h.in
                  ${CMAKE_SOURCE_DIR}/commit.h)
  message ("-- Found SHA1 reference: ${COMMIT}")
endif (EXISTS ${CMAKE_SOURCE_DIR}/.git/index)

set (PACKAGE "${PROJECT_NAME}")
set (VERSION "${PROJECT_VERSION}")
set (PACKAGE_BUGREPORT "support@gothenburgbitfactory.org")
set (PACKAGE_NAME "${PACKAGE}")
set (PACKAGE_TARNAME "${PACKAGE}")
set (PACKAGE_VERSION "${VERSION}")
set (PACKAGE_STRING "${PACKAGE} ${VERSION}")
string (TIMESTAMP PACKAGE_DATE "%Y-%m-%d")

if (FREEBSD OR DRAGONFLY)
set (TIMEW_MANDIR  man      CACHE STRING "Installation directory for man pages")
set (TIMEW_MAN1DIR man/man1 CACHE STRING "Installation directory for man pages, section 1")
set (TIMEW_MAN5DIR man/man5 CACHE STRING "Installation directory for man pages, section 5")
set (TIMEW_MAN7DIR man/man7 CACHE STRING "Installation directory for man pages, section 7")
else (FREEBSD OR DRAGONFLY)
set (TIMEW_MANDIR  share/man      CACHE STRING "Installation directory for man pages")
set (TIMEW_MAN1DIR share/man/man1 CACHE STRING "Installation directory for man pages, section 1")
set (TIMEW_MAN5DIR share/man/man5 CACHE STRING "Installation directory for man pages, section 5")
set (TIMEW_MAN7DIR share/man/man7 CACHE STRING "Installation directory for man pages, section 7")
endif (FREEBSD OR DRAGONFLY)
set (TIMEW_DOCDIR  share/doc/timew CACHE STRING "Installation directory for doc files")

message ("-- Configuring cmake.h")
configure_file (${CMAKE_SOURCE_DIR}/cmake.h.in
                ${CMAKE_SOURCE_DIR}/cmake.h)

add_subdirectory (src)
add_subdirectory (src/commands)
add_subdirectory (doc)
add_subdirectory (ext)
if (EXISTS ${CMAKE_SOURCE_DIR}/test)
  add_subdirectory (test EXCLUDE_FROM_ALL)
endif (EXISTS ${CMAKE_SOURCE_DIR}/test)

set (doc_FILES ChangeLog README.md INSTALL AUTHORS LICENSE)
foreach (doc_FILE ${doc_FILES})
  install (FILES ${doc_FILE}  DESTINATION ${TIMEW_DOCDIR})
endforeach (doc_FILE)

# ---

set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION})
set (CPACK_SOURCE_IGNORE_FILES  "CMakeCache"
                                "CMakeFiles"
                                "CPackConfig"
                                "CPackSourceConfig"
                                "_CPack_Packages"
                                "cmake_install"
                                "install_manifest"
                                "Makefile$"
                                "test"
                                "package-config"
                                "src/timew$"
                                "src/libtimew.a"
                                "src/commands/libcommands.a"
                                "src/lex$"
                                "src/liblibshared.a"
                                "/\\\\.gitignore"
                                "/\\\\.git/"
                                "swp$")
include (CPack)
