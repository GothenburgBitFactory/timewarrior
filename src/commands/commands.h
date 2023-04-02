////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2019, 2022, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <Rules.h>
#include <Database.h>
#include <Extensions.h>
#include <Journal.h>

int CmdAnnotate      (const CLI&, Rules&, Database&, Journal&                   );
int CmdCancel        (            Rules&, Database&, Journal&                   );
int CmdConfig        (const CLI&, Rules&,            Journal&                   );
int CmdContinue      (const CLI&, Rules&, Database&, Journal&                   );
int CmdDefault       (            Rules&, Database&                             );
int CmdDelete        (const CLI&, Rules&, Database&, Journal&                   );
int CmdDiagnostics   (            Rules&, Database&,           const Extensions&);
int CmdExport        (const CLI&, Rules&, Database&                             );
int CmdExtensions    (                                         const Extensions&);
int CmdFill          (const CLI&, Rules&, Database&, Journal&                   );
int CmdGaps          (const CLI&, Rules&, Database&                             );
int CmdGet           (const CLI&, Rules&, Database&                             );
int CmdHelpUsage     (                                         const Extensions&);
int CmdHelp          (const CLI&,                              const Extensions&);
int CmdJoin          (const CLI&, Rules&, Database&, Journal&                   );
int CmdLengthen      (const CLI&, Rules&, Database&, Journal&                   );
int CmdModify        (const CLI&, Rules&, Database&, Journal&                   );
int CmdMove          (const CLI&, Rules&, Database&, Journal&                   );
int CmdReport        (const CLI&, Rules&, Database&,           const Extensions&);
int CmdResize        (const CLI&, Rules&, Database&, Journal&                   );
int CmdShorten       (const CLI&, Rules&, Database&, Journal&                   );
int CmdShow          (            Rules&                                        );
int CmdSplit         (const CLI&, Rules&, Database&, Journal&                   );
int CmdStart         (const CLI&, Rules&, Database&, Journal&                   );
int CmdStop          (const CLI&, Rules&, Database&, Journal&                   );
int CmdTag           (const CLI&, Rules&, Database&, Journal&                   );
int CmdTags          (const CLI&, Rules&, Database&                             );
int CmdTrack         (const CLI&, Rules&, Database&, Journal&                   );
int CmdUndo          (            Rules&, Database&, Journal&                   );
int CmdUntag         (const CLI&, Rules&, Database&, Journal&                   );

int CmdChartDay      (const CLI&, Rules&, Database&                             );
int CmdChartWeek     (const CLI&, Rules&, Database&                             );
int CmdChartMonth    (const CLI&, Rules&, Database&                             );
int CmdSummary       (const CLI&, Rules&, Database&                             );

#endif
