////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2019, 2022 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_COMMANDS
#define INCLUDED_COMMANDS

#include <CLI.h>
#include <Database.h>
#include <Extensions.h>
#include <Journal.h>
#include <Rules.h>

int CmdAnnotate      (CLI&, Rules&, Database&, Journal&                   );
int CmdCancel        (      Rules&, Database&, Journal&                   );
int CmdConfig        (CLI&, Rules&,            Journal&                   );
int CmdContinue      (CLI&, Rules&, Database&, Journal&                   );
int CmdDefault       (      Rules&, Database&                             );
int CmdDelete        (CLI&, Rules&, Database&, Journal&                   );
int CmdDiagnostics   (      Rules&, Database&,           const Extensions&);
int CmdExport        (CLI&, Rules&, Database&                             );
int CmdExtensions    (                                   const Extensions&);
int CmdFill          (CLI&, Rules&, Database&, Journal&                   );
int CmdGaps          (CLI&, Rules&, Database&                             );
int CmdGet           (CLI&, Rules&, Database&                             );
int CmdHelpUsage     (                                   const Extensions&);
int CmdHelp          (CLI&,                              const Extensions&);
int CmdJoin          (CLI&, Rules&, Database&, Journal&                   );
int CmdLengthen      (CLI&, Rules&, Database&, Journal&                   );
int CmdModify        (CLI&, Rules&, Database&, Journal&                   );
int CmdModifyEnd     (CLI&, Rules&, Database&, Journal&                   );
int CmdModifyRange   (CLI&, Rules&, Database&, Journal&                   );
int CmdModifyStart   (CLI&, Rules&, Database&, Journal&                   );
int CmdMove          (CLI&, Rules&, Database&, Journal&                   );
int CmdReport        (CLI&, Rules&, Database&,           const Extensions&);
int CmdResize        (CLI&, Rules&, Database&, Journal&                   );
int CmdRetag         (CLI&, Rules&, Database&, Journal&                   );
int CmdShorten       (CLI&, Rules&, Database&, Journal&                   );
int CmdShow          (      Rules&                                        );
int CmdSplit         (CLI&, Rules&, Database&, Journal&                   );
int CmdStart         (CLI&, Rules&, Database&, Journal&                   );
int CmdStop          (CLI&, Rules&, Database&, Journal&                   );
int CmdTag           (CLI&, Rules&, Database&, Journal&                   );
int CmdTags          (CLI&, Rules&, Database&                             );
int CmdTrack         (CLI&, Rules&, Database&, Journal&                   );
int CmdUndo          (      Rules&, Database&, Journal&                   );
int CmdUntag         (CLI&, Rules&, Database&, Journal&                   );

int CmdChartDay      (CLI&, Rules&, Database&                             );
int CmdChartWeek     (CLI&, Rules&, Database&                             );
int CmdChartMonth    (CLI&, Rules&, Database&                             );
int CmdSummary       (CLI&, Rules&, Database&                             );

#endif
