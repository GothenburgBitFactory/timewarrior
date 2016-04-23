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
#include <Palette.h>
#include <Range.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdReportDay (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  auto filter = createFilterIntervalFromCLI (cli);

  // If filter is empty, choose 'today'.
  if (! filter.range.started ())
    filter.range = Range (Datetime ("today"), Datetime ("tomorrow"));

  auto timeline = createTimelineFromData (rules, database, filter);
  auto tracked = timeline.tracked (rules);

  // Create a color palette.
  Palette palette;
  palette.initialize (rules);
  palette.enabled = rules.getBoolean ("color");

  // Map tags to colors.
  // TODO Note: tags may have their own defined color.
  std::map <std::string, Color> tagMap;

  // TODO Get the exclusion ranges, to render the exclused time.
  // TODO Get the earliest hour of the filtered data.
  // TODO Get the latest hour of the filtered data.

  // TODO Axis, hard-coded.
  std::string indent = "  ";
  std::cout << "\n"
            << indent
            << "0    1    2    3    4    5    6    7    8    9    10   11   12   1    2    3    4    5    6    7    8    9   10   11\n";

  // TODO Data, missing.
  std::cout << "\n"
            << "\n";

  // TODO Summary, missing.
  std::cout << "\n"
            << indent << "Tracked\n"
            << indent << "Untracked\n"
            << indent << "Total\n"
            << "\n";

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
