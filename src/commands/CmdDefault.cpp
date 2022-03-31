////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <IntervalFilterAllInRange.h>
#include <IntervalFilterFirstOf.h>
#include <commands.h>
#include <iostream>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
// Returns 0 if tracking is active, 1 if not.
int CmdDefault (Rules& rules, Database& database)
{
  const bool verbose = rules.getBoolean ("verbose");

  // Load the most recent interval, summarize and display.
  IntervalFilterFirstOf filtering (std::make_shared <IntervalFilterAllInRange> (Range {}));
  auto latest = getTracked (database, rules, filtering);

  if (!latest.empty () && latest.at (0).is_open ())
  {
    if (verbose)
    {
      std::cout << intervalSummarize (rules, latest.at (0));
    }

    return 0;
  }

  if (rules.getBoolean ("temp.shiny"))
  {
    std::cout << '\n'
              << "Welcome to Timewarrior.\n"
              << '\n'
              << "There is built-in help:\n"
              << "    timew help\n"
              << "    timew help <command>\n"
              << "    (and more)\n"
              << '\n'
              << "There is a fully-detailed man page:\n"
              << "    man timew\n"
              << '\n';
    return 0;
  }

  if (verbose)
  {
    std::cout << "There is no active time tracking.\n";
  }

  return 1;
}

////////////////////////////////////////////////////////////////////////////////
