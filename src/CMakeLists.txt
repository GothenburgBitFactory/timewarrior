cmake_minimum_required (VERSION 3.10)
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
                SummaryTable.cpp SummaryTable.h
                TagDescription.cpp TagDescription.h
                TagInfo.cpp    TagInfo.h
                TagInfoDatabase.cpp TagInfoDatabase.h
                TagsTable.cpp TagsTable.h
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

add_subdirectory(${CMAKE_SOURCE_DIR}/src/libshared)
add_library (timew  STATIC ${timew_SRCS})
add_executable (timew_executable timew.cpp)
target_link_libraries (timew_executable timew commands timew shared ${TIMEW_LIBRARIES})
set_property (TARGET timew_executable PROPERTY OUTPUT_NAME "timew")
install (TARGETS timew_executable DESTINATION ${TIMEW_BINDIR})
