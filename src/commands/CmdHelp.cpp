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
#include <commands.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdHelpUsage ()
{
  // TODO This is going to need formatting.
  std::cout << "\n"
            << "Usage: timew [--version]\n"
            << "       timew continue\n"
            << "       timew diagnostics\n"
            << "       timew export\n"
            << "       timew extensions\n"
            << "       timew start [<tag> ...]\n"
            << "       timew stop\n"
            << "       timew tags\n"
            << "\n";

  // TODO clear
  // TODO config
  // TODO define
  // TODO gaps
  // TODO help
  // TODO import
  // TODO report
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
int CmdHelp (CLI& cli, Log& log)
{
  auto words = cli.getWords ();
  if (words.size ())
  {
    // TODO clear
    // TODO config

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    if (words[0] == "continue")
      std::cout << "\n"
                << "Syntax: timew continue\n"
                << "\n"
                << "Resumes tracking the most recently closed interval. For exqample:\n"
                << "\n"
                << "  $ timew track yesterday 9am - 5pm tag1 tag2\n"
                << "  $ timew continue\n"
                << "\n"
                << "The 'continue' command creates a new interval, starting now, and using the tags\n"
                << "'tag1' and 'tag2'.\n"
                << "\n"
                << "See also 'start', 'stop'.\n"
                << "\n";
    // TODO define

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "diagnostics")
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

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "export")
      std::cout << "\n"
                << "Syntax: timew export\n"
                << "\n"
                << "Exports all the tracked time in JSON format.\n"
                << "\n"
                << "See also 'import'.\n"
                << "\n";

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "extensions")
      std::cout << "\n"
                << "Syntax: timew extensions\n"
                << "\n"
                << "Displays the directory containing the extension programs, and a table showing\n"
                << "each extensions and its status.\n"
                << "\n"
                << "See also 'diagnostics'.\n"
                << "\n";

    // TODO gaps
    // TODO help
    // TODO import
    // TODO report

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "start")
      std::cout << "\n"
                << "Syntax: timew start [<tag> ...]\n"
                << "\n"
                << "Begins tracking using the current time, and the specified optional set of tags.\n"
                << "If a tag contains multiple words, and therefore contains spaces, use quotes to\n"
                << "surround the whole tag. For example, this command specifies two tags ('weekend'\n"
                << "and 'Home & Garden'), the second of which requires quotes.\n"
                << "\n"
                << "  $ timew start weekend 'Home & Garden'\n"
                << "\n"
                << "Quotes are harmless if used every time.\n"
                << "\n"
                << "See also 'continue', 'stop', 'track'.\n"
                << "\n";

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "stop")
      std::cout << "\n"
                << "Syntax: timew stop [<tag> ...]\n"
                << "\n"
                << "Stops tracking time.  If tags are specified, then they are no longer tracked.\n"
                << "If no tags are specified, all tracking stops. For example:\n"
                << "\n"
                << "  $ timew start tag1 tag2\n"
                << "  ...\n"
                << "  $ timew stop tag1\n"
                << "\n"
                << "Initially time is tracked for both 'tag1' and 'tag2', then 'tag1' tracking is\n"
                << "stopped, leaving tag2 active. To stop all tracking:\n"
                << "\n"
                << "  $ timew stop\n"
                << "\n"
                << "See also 'continue', 'start', 'track'.\n"
                << "\n";

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "tags")
      std::cout << "\n"
                << "Syntax: timew tags\n"
                << "\n"
                << "Displays all the tags that have been used.\n"
                << "\n";

    // TODO track
    // TODO undo

    else
      std::cout << "No help available for '" << words[0] << "'\n";

    return 0;
  }

  return CmdHelpUsage ();
}

////////////////////////////////////////////////////////////////////////////////
