cmake_minimum_required (VERSION 2.8)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/commands
                     ${TIMEW_INCLUDE_DIRS})

set (commands_SRCS CmdClear.cpp
                   CmdDefault.cpp
                   CmdDefine.cpp
                   CmdExport.cpp
                   CmdHelp.cpp
                   CmdImport.cpp
                   CmdReport.cpp
                   CmdStart.cpp
                   CmdStop.cpp
                   CmdTrack.cpp)

add_library (commands STATIC ${commands_SRCS})

