cmake_minimum_required (VERSION 2.8)

# This is a work-around for the following CMake issue:
# https://gitlab.kitware.com/cmake/cmake/issues/16062
# The issue has been fixed in CMake 3.11.0; the policy is set
# to OLD for compatibility with older versions of CMake only.
if(POLICY CMP0037 AND ${CMAKE_VERSION} VERSION_LESS "3.11.0")
  cmake_policy(SET CMP0037 OLD)
endif()

# If this is a debug build, check if we have libfiu installed and available on
# the system. If so, we will be able to use it to add additional tests of the
# failure conditions

if (uppercase_CMAKE_BUILD_TYPE MATCHES "DEBUG")
  find_library(FIU_ENABLE fiu) 
  if (FIU_ENABLE)
    message (STATUS "libfiu found")
    add_definitions (-DFIU_ENABLE)
    set (test_LIBS fiu ${TIMEW_LIBRARIES})
  else (FIU_ENABLE)
    message (STATUS "NOTE: install libfiu to run additional tests")
    set (test_LIBS ${TIMEW_LIBRARIES})
  endif (FIU_ENABLE)
else (uppercase_CMAKE_BUILD_TYPE MATCHES "DEBUG")
  set (test_LIBS ${TIMEW_LIBRARIES})
endif (uppercase_CMAKE_BUILD_TYPE MATCHES "DEBUG")

include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/libshared/src
                     ${CMAKE_SOURCE_DIR}/test)

include_directories (${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

set (test_SRCS AtomicFileTest data.t DatetimeParser.t exclusion.t helper.t interval.t range.t rules.t util.t TagInfoDatabase.t)

add_custom_target (test ./run_all --verbose
                        DEPENDS ${test_SRCS}
                        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test)

foreach (src_FILE ${test_SRCS})
  add_executable (${src_FILE} "${src_FILE}.cpp" test.cpp)
  target_link_libraries (${src_FILE} timew libshared ${test_LIBS})
endforeach (src_FILE)

configure_file(run_all run_all COPYONLY)
configure_file(problems problems COPYONLY)
