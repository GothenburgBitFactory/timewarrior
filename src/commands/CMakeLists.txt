cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/commands
                     ${CMAKE_SOURCE_DIR}/src/libshared/src
                     ${TIMEW_INCLUDE_DIRS})

set (commands_SRCS CmdCancel.cpp
                   CmdChart.cpp
                   CmdConfig.cpp
                   CmdContinue.cpp
                   CmdDefault.cpp
                   CmdDelete.cpp
                   CmdDiagnostics.cpp
                   CmdExport.cpp
                   CmdExtensions.cpp
                   CmdGaps.cpp
                   CmdGet.cpp
                   CmdHelp.cpp
                   CmdJoin.cpp
                   CmdLengthen.cpp
                   CmdMove.cpp
                   CmdReport.cpp
                   CmdStart.cpp
                   CmdStop.cpp
                   CmdSummary.cpp
                   CmdShorten.cpp
                   CmdShow.cpp
                   CmdSplit.cpp
                   CmdTag.cpp
                   CmdTags.cpp
                   CmdTrack.cpp
                   CmdUntag.cpp)

add_library (commands STATIC ${commands_SRCS})

