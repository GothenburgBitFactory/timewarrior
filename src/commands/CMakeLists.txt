cmake_minimum_required (VERSION 2.8.12)
include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/commands
                     ${CMAKE_SOURCE_DIR}/src/libshared/src
                     ${TIMEW_INCLUDE_DIRS})

set (ADDITIONAL_HELP_H "additional-help.h")

ADD_CUSTOM_COMMAND (
	OUTPUT ${ADDITIONAL_HELP_H}
	COMMAND /bin/sh src/commands/generate-additional-help.sh > src/commands/${ADDITIONAL_HELP_H}
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/generate-additional-help.sh
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Generating ${ADDITIONAL_HELP_H}")

ADD_CUSTOM_TARGET (generate_additional_help
									 DEPENDS ${ADDITIONAL_HELP_H})

set (commands_SRCS CmdAnnotate.cpp
                   CmdCancel.cpp
                   CmdChart.cpp
                   CmdConfig.cpp
                   CmdContinue.cpp
                   CmdDefault.cpp
                   CmdDelete.cpp
                   CmdDiagnostics.cpp
                   CmdExport.cpp
                   CmdExtensions.cpp
                   CmdFill.cpp
                   CmdGaps.cpp
                   CmdGet.cpp
                   CmdHelp.cpp
                   CmdJoin.cpp
                   CmdLengthen.cpp
                   CmdModify.cpp
                   CmdMove.cpp
                   CmdReport.cpp
                   CmdResize.cpp
                   CmdStart.cpp
                   CmdStop.cpp
                   CmdSummary.cpp
                   CmdShorten.cpp
                   CmdShow.cpp
                   CmdSplit.cpp
                   CmdTag.cpp
                   CmdTags.cpp
                   CmdTrack.cpp
                   CmdUndo.cpp
                   CmdUntag.cpp)

add_library (commands STATIC ${commands_SRCS})
add_dependencies (commands generate_additional_help)
