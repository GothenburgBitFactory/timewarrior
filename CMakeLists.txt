cmake_minimum_required (VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required

include (CheckFunctionExists)
include (CheckStructHasMember)

set (HAVE_CMAKE true)

project (timew)
set (PROJECT_VERSION "1.0.0")

message ("CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}")

include (CheckCXXCompilerFlag)

# NOTE: If we are to actually use C++11 features, we should either require
#       a compiler that supports the -std=c++11 flag or check for the
#       features used.
#       Relying on -std=c++0x or even -std=gnu++0x is highly volatile.

CHECK_CXX_COMPILER_FLAG("-std=c++11" _HAS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" _HAS_CXX0X)
CHECK_CXX_COMPILER_FLAG("-std=gnu++0x" _HAS_GNU0X)

if (_HAS_CXX11)
  set (_CXX11_FLAGS "-std=c++11")
elseif (_HAS_CXX0X)
  message (WARNING "Enabling -std=c++0x draft compile flag. Your compiler does not support the standard '-std=c++11' option.  Consider upgrading.")
  set (_CXX11_FLAGS "-std=c++0x")
elseif (_HAS_GNU0X)
  message (WARNING "Enabling -std=gnu++0x draft compile flag. Your compiler does not support the standard '-std=c++11' option. Consider upgrading.")
  set (_CXX11_FLAGS "-std=gnu++0x")
else (_HAS_CXX11)
 message (FATAL_ERROR "C++11 support missing. Try upgrading your C++ compiler. If you have a good reason for using an outdated compiler, please let us know at support@taskwarrior.org.")
endif (_HAS_CXX11)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set (LINUX true)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (DARWIN true)
  set (_CXX11_FLAGS "${_CXX11_FLAGS} -stdlib=libc++")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "kFreeBSD")
  set (KFREEBSD true)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
  set (FREEBSD true)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "OpenBSD")
  set (OPENBSD true)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "NetBSD")
  set (NETBSD true)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
  set (SOLARIS true)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "GNU")
  set (GNUHURD true)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "CYGWIN")
  set (CYGWIN true)
  # NOTE: Not setting -std=gnu++0x leads to compile errors even with
  #       GCC 4.8.3, and debugging those leads to insanity. Adding this
  #       workaround instead of fixing Cygwin.
  set (_CXX11_FLAGS "-std=gnu++0x")
else (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set (UNKNOWN true)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set (CMAKE_CXX_FLAGS "${_CXX11_FLAGS} ${CMAKE_CXX_FLAGS}")

set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wsign-compare -Wreturn-type ${CMAKE_CXX_FLAGS}")

if (FREEBSD)
SET (TIMEW_MAN1DIR man/man1 CACHE STRING "Installation directory for man pages, section 1")
SET (TIMEW_MAN5DIR man/man5 CACHE STRING "Installation directory for man pages, section 5")
else (FREEBSD)
SET (TIMEW_MAN1DIR share/man/man1 CACHE STRING "Installation directory for man pages, section 1")
SET (TIMEW_MAN5DIR share/man/man5 CACHE STRING "Installation directory for man pages, section 5")
endif (FREEBSD)
SET (TIMEW_DOCDIR  share/doc/timew CACHE STRING "Installation directory for doc files")
SET (TIMEW_RCDIR "${TIMEW_DOCDIR}/rc" CACHE STRING "Installation directory for configuration files")
SET (TIMEW_BINDIR  bin            CACHE STRING "Installation directory for the binary")

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

message ("-- Configuring cmake.h")
configure_file (
  ${CMAKE_SOURCE_DIR}/cmake.h.in
  ${CMAKE_SOURCE_DIR}/cmake.h)

add_subdirectory (src)
add_subdirectory (src/common)
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
                                "test" "package-config" "misc/*" "src/timew$" "src/common/libcommon.a"
                                "performance" "src/libtimew.a" "/\\\\.gitignore" "/\\\\.git/" "swp$"
                                "src/lex$")
include (CPack)
