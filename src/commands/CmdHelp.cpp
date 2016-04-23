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
            << "       timew export [<interval>] [<tag> ...]\n"
            << "       timew extensions\n"
            << "       timew help [<command> | hints]\n"
            << "       timew [report] <report> [<interval>] [<tag> ...]\n"
            << "       timew start [<date>] [<tag> ...]\n"
            << "       timew stop [<tag> ...]\n"
            << "       timew tags\n"
            << "       timew track <interval> [<tag> ...]\n"
            << "\n"
            << "Interval:\n"
            << "       [from] <date>\n"
            << "       [from] <date> to/- <date>\n"
            << "       [from] <date> for <duration>\n"
            << "       <duration> before <date>\n"
            << "       <duration> after <date>\n"
            << "       <duration>\n"
            << "\n"
            << "Tag:\n"
            << "       Word\n"
            << "       'Single Quoted Words'\n"
            << "       \"Double Quoted Words\"\n"
            << "       Escaped\\ Spaces\n"
            << "\n";

  // TODO clear
  // TODO config
  // TODO day
  // TODO gaps
  // TODO import
  // TODO month
  // TODO quarter
  // TODO report
  // TODO summary
  // TODO undo
  // TODO week
  // TODO year

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
int CmdHelp (const CLI& cli)
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
                << "  $ timew track 9am - 5pm tag1 tag2\n"
                << "  $ timew continue\n"
                << "\n"
                << "The 'continue' command creates a new interval, starting now, and using the tags\n"
                << "'tag1' and 'tag2'.\n"
                << "\n"
                << "See also 'start', 'stop'.\n"
                << "\n";

    // TODO day

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
                << "Syntax: timew export [<interval>] [<tag> ...]\n"
                << "\n"
                << "Exports all the tracked time in JSON format. Supports filtering. For example:\n"
                << "\n"
                << "  $ timew export from 2016-01-01 for 3wks tag1\n"
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

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "help")
      std::cout << "\n"
                << "The help command shows detailed descriptions and examples of commands and the\n"
                << "supported hints. For example:\n"
                << "\n"
                << "  $ timew help\n"
                << "  $ timew help start\n"
                << "  $ timew help hints\n"
                << "\n";

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "hints")
      std::cout << "\n"
                << "Timewarrior supports hints, which are single-word command line features that\n"
                << "start with a colon like this:\n"
                << "\n"
                << "  :week\n"
                << "\n"
                << "Hints serve several purposes. This example is a shortcut for the date range\n"
                << "that defines the current week. Other hints, such as:\n"
                << "\n"
                << "  :quiet\n"
                << "\n"
                << "are ways to control the behavior of Timewarrior, in this case eliminating all\n"
                << "forms of feedback, for purposes of automation. The supported hints are:\n"
                << "\n"
                << "  :quiet         Turns off all feedback. For automation\n"
                << "  :debug         Runs in debug mode, shows many runtime details\n"
                << "\n"
                << "  :color         Force color on, even if not connected to a TTY\n"
                << "  :nocolor       Force color off, even if connected to a TTY\n"
                << "\n"
                << "  :yesterday     The 24 hours of the previous day\n"
                << "  :day           The 24 hours of the current day\n"
                << "  :week          This week\n"
                << "  :month         This month\n"
                << "  :quarter       This quarter\n"
                << "  :year          This year\n"
                << "\n"
                << "  :fill          Expand time to fill surrounding available gap\n"
                << "                 Only functions when exclusions are provided\n"
                << "\n";

    // TODO import
    // TODO month
    // TODO quarter
    // TODO report

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "start")
      std::cout << "\n"
                << "Syntax: timew start [<date>] [<tag> ...]\n"
                << "\n"
                << "Begins tracking using the current time, and the specified optional set of tags.\n"
                << "If a tag contains multiple words, and therefore contains spaces, use quotes to\n"
                << "surround the whole tag. For example, this command specifies two tags ('weekend'\n"
                << "and 'Home & Garden'), the second of which requires quotes.\n"
                << "\n"
                << "  $ timew start weekend 'Home & Garden'\n"
                << "\n"
                << "An optional date may be specified to indicate the intended start to the tracked\n"
                << "time:\n"
                << "\n"
                << "  $ time start 8am weekend 'Home & Garden'\n"
                << "\n"
                << "Quotes are harmless if used unecessarily.\n"
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

    // TODO summary

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
    // TODO week
    // TODO year

    else
      std::cout << "No help available for '" << words[0] << "'\n";

    return 0;
  }

  return CmdHelpUsage ();
}

////////////////////////////////////////////////////////////////////////////////
