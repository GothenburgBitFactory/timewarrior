////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2023, Gothenburg Bit Factory.
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

#include <Duration.h>
#include <GapsTable.h>
#include <commands.h>
#include <iostream>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
int CmdGaps (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  const bool verbose = rules.getBoolean ("verbose");

  auto default_hint = rules.get ("reports.range", "day");
  auto report_hint = rules.get ("reports.gaps.range", default_hint);

  Range default_range = {};
  expandIntervalHint (":" + report_hint, default_range);

  auto range = cli.getRange (default_range);
  auto tags = cli.getTags ();
  auto filter = Interval {range, tags};

  // Is the :blank hint being used?
  bool blank = cli.getHint ("blank", false);

  std::vector <Range> untracked;
  if (blank)
  {
    untracked = subtractRanges ({range}, getAllExclusions (rules, range));
  }
  else
  {
    untracked = getUntracked (database, rules, filter);
  }

  if (untracked.empty ())
  {
    if (verbose)
    {
      std::cout << "No gaps found.\n";
    }
  }
  else
  {
    auto table = GapsTable::builder ()
      .withRange (range)
      .withIntervals (untracked)
      .build ();

    std::cout << '\n'
              << table.render ()
              << '\n';
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
