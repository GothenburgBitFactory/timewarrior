////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <Table.h>
#include <Duration.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdGaps (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  const Datetime NOW {};
  auto verbose = rules.getBoolean ("verbose");

  // If filter is empty, choose 'today'.
  auto filter = cli.getFilter ();
  if (! filter.is_started ())
  {
    if (rules.has ("reports.gaps.range"))
      expandIntervalHint (rules.get ("reports.gaps.range"), filter);
    else
      filter.setRange (Datetime ("today"), Datetime ("tomorrow"));
  }

  // Is the :blank hint being used?
  bool blank = findHint (cli, ":blank");

  std::vector <Range> untracked;
  if (blank)
    untracked = subtractRanges ({filter}, getAllExclusions (rules, filter));
  else
    untracked = getUntracked (database, rules, filter);

  Table table;
  table.width (1024);
  table.colorHeader (Color ("underline"));
  table.add ("Wk");
  table.add ("Date");
  table.add ("Day");
  table.add ("Start", false);
  table.add ("End", false);
  table.add ("Time", false);
  table.add ("Total", false);

  // Each day is rendered separately.
  time_t grand_total = 0;
  Datetime previous;
  auto it = untracked.cbegin ();
  auto end = untracked.cend ();
  for (Datetime day = filter.start; day < filter.end; )
  {
    auto day_range = getFullDay (day);
    time_t daily_total = 0;

    int row = -1;
    decltype (it) day_end;
    std::tie (it, day_end) = intersects (it, end, day_range);
    if (it == day_end)
    {
      if (it == end)
      {
         // no more gaps to process
         break;
      }

      // Advance day to one where we will have gaps.
      day = day_end->start;
      continue;
    }

    for (auto gap_it = it; gap_it != day_end; ++gap_it)
    {
      const Range& gap = *gap_it;

      row = table.addRow ();

      if (day != previous)
      {
        table.set (row, 0, format ("W{1}", day.week ()));
        table.set (row, 1, day.toString ("Y-M-D"));
        table.set (row, 2, day.dayNameShort (day.dayOfWeek ()));
        previous = day;
      }

      // Intersect track with day.
      auto today = day_range.intersect (gap);
      if (gap.is_open ())
      {
        today.end = NOW;
      }

      table.set (row, 3, today.start.toString ("h:N:S"));
      table.set (row, 4, (gap.is_open () ? "-" : today.end.toString ("h:N:S")));
      table.set (row, 5, Duration (today.total ()).formatHours ());

      daily_total += today.total ();
    }

    if (row != -1)
      table.set (row, 6, Duration (daily_total).formatHours ());

    grand_total += daily_total;
    ++day;
  }

  // Add the total.
  table.set (table.addRow (), 6, " ", Color ("underline"));
  table.set (table.addRow (), 6, Duration (grand_total).formatHours ());

  if (table.rows () > 2)
  {
    std::cout << '\n'
              << table.render ()
              << '\n';
  }
  else
  {
    if (verbose)
      std::cout << "No gaps found.\n";
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
