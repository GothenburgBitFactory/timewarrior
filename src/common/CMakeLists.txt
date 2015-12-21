cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src/common
                     ${TIMEW_INCLUDE_DIRS})

set (common_SRCS Color.cpp    Color.h
                 FS.cpp       FS.h
                 RX.cpp       RX.h
                 Timer.cpp    Timer.h
                 text.cpp     text.h
                 utf8.cpp     utf8.h
                 wcwidth6.cpp)

add_library (common STATIC ${common_SRCS})

#SET(CMAKE_BUILD_TYPE gcov)
#SET(CMAKE_CXX_FLAGS_GCOV "--coverage")
#SET(CMAKE_C_FLAGS_GCOV "--coverage")
#SET(CMAKE_EXE_LINKER_FLAGS_GCOV "--coverage")
