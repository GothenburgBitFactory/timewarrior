cmake_minimum_required (VERSION 2.8)
set (CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set (HAVE_CMAKE true)

project (timew)
include (CXXSniffer)
include (ExternalProject)

set (PROJECT_VERSION "0.1.0")

message ("-- Looking for SHA1 references")
if (EXISTS ${CMAKE_SOURCE_DIR}/.git/index)
  set (HAVE_COMMIT true)
  execute_process (COMMAND git log -1 --pretty=format:%h
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                   OUTPUT_VARIABLE COMMIT)
  configure_file ( ${CMAKE_SOURCE_DIR}/commit.h.in
                   ${CMAKE_SOURCE_DIR}/commit.h)
  message ("-- Found SHA1 reference: ${COMMIT}")
endif (EXISTS ${CMAKE_SOURCE_DIR}/.git/index)

set (PACKAGE "${PROJECT_NAME}")
set (VERSION "${PROJECT_VERSION}")
set (PACKAGE_BUGREPORT "support@taskwarrior.org")
set (PACKAGE_NAME "${PACKAGE}")
set (PACKAGE_TARNAME "${PACKAGE}")
set (PACKAGE_VERSION "${VERSION}")
set (PACKAGE_STRING "${PACKAGE} ${VERSION}")

if (FREEBSD)
SET (TIMEW_MAN1DIR man/man1 CACHE STRING "Installation directory for man pages, section 1")
SET (TIMEW_MAN5DIR man/man5 CACHE STRING "Installation directory for man pages, section 5")
else (FREEBSD)
SET (TIMEW_MAN1DIR share/man/man1 CACHE STRING "Installation directory for man pages, section 1")
SET (TIMEW_MAN5DIR share/man/man5 CACHE STRING "Installation directory for man pages, section 5")
endif (FREEBSD)
SET (TIMEW_DOCDIR  share/doc/timew CACHE STRING "Installation directory for doc files")

message ("-- Configuring cmake.h")
configure_file (
  ${CMAKE_SOURCE_DIR}/cmake.h.in
  ${CMAKE_SOURCE_DIR}/cmake.h)

ExternalProject_Add (common-1.0
                     GIT_REPOSITORY      "ssh://git@git.tasktools.org/tm/common.git"
                     GIT_TAG             "master"
                     #UPDATE_DISCONNECTED 1 # Needs 3.2
                     )

add_subdirectory (src)
add_subdirectory (src/commands)
add_subdirectory (doc)
if (EXISTS ${CMAKE_SOURCE_DIR}/test)
  add_subdirectory (test EXCLUDE_FROM_ALL)
endif (EXISTS ${CMAKE_SOURCE_DIR}/test)

set (doc_FILES NEWS ChangeLog README.md INSTALL AUTHORS COPYING LICENSE)
foreach (doc_FILE ${doc_FILES})
  install (FILES ${doc_FILE}  DESTINATION ${TIMEW_DOCDIR})
endforeach (doc_FILE)

# ---

set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE_NAME}-${PACKAGE_VERSION})
set (CPACK_SOURCE_IGNORE_FILES  "CMakeCache" "CMakeFiles" "CPackConfig" "CPackSourceConfig"
                                "_CPack_Packages" "cmake_install" "install_manifest" "Makefile$"
                                "test" "package-config" "src/timew$" "src/libtimew.a"
                                "src/commands/libcommands.a"
                                "/\\\\.gitignore" "/\\\\.git/" "swp$" "src/gr$")
include (CPack)
