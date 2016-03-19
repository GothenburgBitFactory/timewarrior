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
            << "       timew log [<message>]\n"
            << "\n";

  // TODO List all commands.
  // TODO List all extensions.

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TODO Perhaps have a set of 'documents' to display, and a keyword index that
//      maps to the the appropriate ones. Then display a list of any ambiguous
//      requests, or the appropriate doc for single hits. Perhaps a 'See also'.
//
// Strict 80-character limit.
int CmdHelp (const std::vector <std::string>& args, Log& log)
{
  if (args.size () > 2)
  {
    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    if (args[2] == "log")
      std::cout << "\n"
                << "timew log [<message>]\n"
                << "\n"
                << "Writes the info <message> to the Timewarrior log file. The <message> may be\n"
                << "multiple words, or a quoted string. If <message> is ommitted, writes a cut\n"
                << "marker ('-------- 8< --------') line to the log file.\n"
                << "\n"
                << "Note: Your log file is found here:\n"
                << "  " << log.file () << "\n"
                << "\n";
    else
      std::cout << "No help available for '" << args[2] << "'\n";

    return 0;
  }

  return CmdHelpUsage ();
}

////////////////////////////////////////////////////////////////////////////////
