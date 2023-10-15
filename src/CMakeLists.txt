cmake_minimum_required (VERSION 3.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/commands
                     ${CMAKE_SOURCE_DIR}/src/libshared/src
                     ${TIMEW_INCLUDE_DIRS})

set (timew_SRCS AtomicFile.cpp AtomicFile.h
                CLI.cpp        CLI.h
                Chart.cpp      Chart.h
                               ChartConfig.h
                Database.cpp   Database.h
                Datafile.cpp   Datafile.h
                DatetimeParser.cpp DatetimeParser.h
                Exclusion.cpp  Exclusion.h
                Extensions.cpp Extensions.h
                ExtensionsTable.cpp ExtensionsTable.h
                GapsTable.cpp GapsTable.h
                Interval.cpp   Interval.h
                IntervalFactory.cpp IntervalFactory.h
                IntervalFilter.cpp IntervalFilter.h
                IntervalFilterAndGroup.cpp IntervalFilterAndGroup.h
                IntervalFilterAllInRange.cpp IntervalFilterAllInRange.h
                IntervalFilterAllWithIds.cpp IntervalFilterAllWithIds.h
                IntervalFilterAllWithTags.cpp IntervalFilterAllWithTags.h
                IntervalFilterFirstOf.cpp IntervalFilterFirstOf.h
                Journal.cpp    Journal.h
                Range.cpp      Range.h
                Rules.cpp      Rules.h
                TagInfo.cpp    TagInfo.h
                TagInfoDatabase.cpp TagInfoDatabase.h
                Transaction.cpp Transaction.h
                TransactionsFactory.cpp TransactionsFactory.h
                UndoAction.cpp UndoAction.h
                data.cpp
                dom.cpp
                init.cpp
                helper.cpp
                paths.cpp      paths.h
                log.cpp
                util.cpp
                validate.cpp)

set (libshared_SRCS libshared/src/Args.cpp          libshared/src/Args.h
                    libshared/src/Color.cpp         libshared/src/Color.h
                    libshared/src/Composite.cpp     libshared/src/Composite.h
                    libshared/src/Configuration.cpp libshared/src/Configuration.h
                    libshared/src/Datetime.cpp      libshared/src/Datetime.h
                    libshared/src/Duration.cpp      libshared/src/Duration.h
                    libshared/src/FS.cpp            libshared/src/FS.h
                    libshared/src/JSON.cpp          libshared/src/JSON.h
                    libshared/src/Lexer.cpp         libshared/src/Lexer.h
                    libshared/src/Msg.cpp           libshared/src/Msg.h
                    libshared/src/Palette.cpp       libshared/src/Palette.h
                    libshared/src/Pig.cpp           libshared/src/Pig.h
                    libshared/src/RX.cpp            libshared/src/RX.h
                    libshared/src/Table.cpp         libshared/src/Table.h
                    libshared/src/Timer.cpp         libshared/src/Timer.h
                    libshared/src/format.cpp        libshared/src/format.h
                    libshared/src/shared.cpp        libshared/src/shared.h
                    libshared/src/unicode.cpp       libshared/src/unicode.h
                    libshared/src/utf8.cpp          libshared/src/utf8.h
                                                    libshared/src/wcwidth.h)

add_library (timew     STATIC ${timew_SRCS})
add_library (libshared STATIC ${libshared_SRCS})
add_executable (timew_executable timew.cpp)
add_executable (lex_executable   lex.cpp)

target_link_libraries (timew_executable timew libshared commands timew libshared ${TIMEW_LIBRARIES})
target_link_libraries (lex_executable   timew libshared                libshared ${TIMEW_LIBRARIES})

set_property (TARGET timew_executable PROPERTY OUTPUT_NAME "timew")
set_property (TARGET lex_executable   PROPERTY OUTPUT_NAME "lex")

install (TARGETS timew_executable DESTINATION ${TIMEW_BINDIR})
