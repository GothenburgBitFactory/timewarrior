////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, Paul Beckingham, Federico Hernandez.
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
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_COMMANDS
#define INCLUDED_COMMANDS

#include <CLI.h>
#include <Rules.h>
#include <Database.h>
#include <Extensions.h>
#include <Log.h>
#include <vector>
#include <string>

int CmdClear       (                                          );
int CmdConfig      (                                          );
int CmdContinue    (      Rules&, Database&,              Log&);
int CmdDefault     (      Rules&, Database&                   );
int CmdDefine      (const std::vector <std::string>&, Rules&, Database&,              Log&);
int CmdDiagnostics (      Rules&, Database&, Extensions&, Log&);
int CmdExport      (CLI&, Rules&, Database&,              Log&);
int CmdExtensions  (      Rules&,            Extensions&      );
int CmdGaps        (                                          );
int CmdHelpUsage   (                                          );
int CmdHelp        (CLI&,                                 Log&);
int CmdLog         (CLI&,                                 Log&);
int CmdImport      (                                          );
int CmdReport      (CLI&, Rules&, Database&, Extensions&, Log&);
int CmdStart       (const std::vector <std::string>&, Rules&, Database&,              Log&);
int CmdStop        (const std::vector <std::string>&, Rules&, Database&,              Log&);
int CmdTags        (      Rules&, Database&,              Log&);
int CmdTrack       (                                          );
int CmdUndo        (                                          );

#endif
