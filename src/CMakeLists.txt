cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/commands
                     ${CMAKE_SOURCE_DIR}/src/libshared/src
                     ${TIMEW_INCLUDE_DIRS})

set (timew_SRCS Database.cpp   Database.h
                Datafile.cpp   Datafile.h
                Extensions.cpp Extensions.h
                Grammar.cpp    Grammar.h
                Interval.cpp   Interval.h
                Lexer.cpp      Lexer.h
                LR0.cpp        LR0.h
                Rules.cpp      Rules.h
                init.cpp
                util.cpp)

set (libshared_SRCS libshared/src/Args.cpp          libshared/src/Args.h
                    libshared/src/Color.cpp         libshared/src/Color.h
                    libshared/src/Configuration.cpp libshared/src/Configuration.h
                    libshared/src/Datetime.cpp      libshared/src/Datetime.h
                    libshared/src/Duration.cpp      libshared/src/Duration.h
                    libshared/src/FS.cpp            libshared/src/FS.h
                    libshared/src/JSON.cpp          libshared/src/JSON.h
                    libshared/src/Log.cpp           libshared/src/Log.h
                    libshared/src/Msg.cpp           libshared/src/Msg.h
                    libshared/src/Pig.cpp           libshared/src/Pig.h
                    libshared/src/RX.cpp            libshared/src/RX.h
                    libshared/src/Table.cpp         libshared/src/Table.h
                    libshared/src/Timer.cpp         libshared/src/Timer.h
                    libshared/src/format.cpp        libshared/src/format.h
                    libshared/src/shared.cpp        libshared/src/shared.h
                    libshared/src/unicode.cpp       libshared/src/unicode.h
                    libshared/src/utf8.cpp          libshared/src/utf8.h
                    libshared/src/wcwidth6.cpp)

add_library (timew     STATIC ${timew_SRCS})
add_library (libshared STATIC ${libshared_SRCS})
add_executable (timew_executable timew.cpp)
add_executable (gr_executable gr.cpp)

target_link_libraries (timew_executable timew libshared commands timew libshared ${TIMEW_LIBRARIES})
target_link_libraries (gr_executable    timew                          libshared ${TIMEW_LIBRARIES})

set_property (TARGET timew_executable PROPERTY OUTPUT_NAME "timew")
set_property (TARGET gr_executable    PROPERTY OUTPUT_NAME "gr")

install (TARGETS timew_executable DESTINATION bin)

