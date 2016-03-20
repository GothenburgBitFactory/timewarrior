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

#include <cmake.h>
#include <iostream>
#include <commands.h>

////////////////////////////////////////////////////////////////////////////////
int CmdHelpUsage ()
{
  // TODO This is going to need formatting.
  std::cout << "\n"
            << "Usage: timew [--version]\n"
            << "       timew diagnostics\n"
            << "       timew extensions\n"
            << "       timew log [<message>]\n"
            << "\n";

  // TODO clear
  // TODO config
  // TODO continue
  // TODO define
  // TODO export
  // TODO gaps
  // TODO help
  // TODO import
  // TODO report
  // TODO start
  // TODO stop
  // TODO tags
  // TODO track
  // TODO undo

  // TODO List all extensions.

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TODO Perhaps have a set of 'documents' to display, and a keyword index that
//      maps to the the appropriate ones. Then display a list of any ambiguous
//      requests, or the appropriate doc for single hits. Perhaps a 'See also'.
//
// TODO Should the CmdXxx functions themselves be responsible for providing both
//      the command syntax and the help text?
//
// Strict 80-character limit.
// Provide examples where appropriate - enough to cover all uses.
int CmdHelp (const std::vector <std::string>& args, Log& log)
{
  if (args.size () > 2)
  {
    // TODO clear
    // TODO config
    // TODO continue
    // TODO define

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    if (args[2] == "diagnostics")
      std::cout << "\n"
                << "Syntax: timew diagnostics\n"
                << "\n"
                << "This command shows details about your version of Timewarrior, platform, how it\n"
                << "was built, compiler features, configuration, file access, extensions and more.\n"
                << "\n"
                << "The purpose of this command is to help diagnose configuration problems, and\n"
                << "provide supplemental information when reporting a problem.\n"
                << "\n"
                << "See also 'extensions'.\n"
                << "\n";

    // TODO export

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (args[2] == "extensions")
      std::cout << "\n"
                << "Syntax: timew extensions\n"
                << "\n"
                << "Displays the directory containing the extension programs, and a table with each\n"
                << "of the extensions and status.\n"
                << "\n"
                << "See also 'diagnostics'.\n"
                << "\n";

    // TODO gaps
    // TODO help
    // TODO import

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (args[2] == "log")
      std::cout << "\n"
                << "Syntax: timew log [<message>]\n"
                << "\n"
                << "Writes the info <message> to the Timewarrior log file. The <message> may be\n"
                << "multiple words, or a quoted string. If <message> is ommitted, writes a cut\n"
                << "marker ('-------- 8< --------') line to the log file.\n"
                << "\n"
                << "Note: Your log file is found here:\n"
                << "  " << log.file () << "\n"
                << "\n";

    // TODO report
    // TODO start
    // TODO stop
    // TODO tags
    // TODO track
    // TODO undo

    else
      std::cout << "No help available for '" << args[2] << "'\n";

    return 0;
  }

  return CmdHelpUsage ();
}

////////////////////////////////////////////////////////////////////////////////
