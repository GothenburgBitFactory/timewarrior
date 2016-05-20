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
#include <Table.h>
#include <Duration.h>
#include <shared.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdSummary (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose 'today'.
  auto filter = getFilter (cli);
  if (! filter.range.is_started ())
    filter.range = Range (Datetime ("today"), Datetime ("tomorrow"));

  // Load the data.
  auto tracked = getTracked (database, rules, filter);

  // Map tags to colors.
  auto palette = createPalette (rules);
  auto tag_colors = createTagColorMap (rules, palette, tracked);

  Table table;
  table.width (1024);
  table.colorHeader (Color ("underline"));
  table.add ("Wk");
  table.add ("Date");
  table.add ("Day");
  table.add ("Tags");
  table.add ("Start", false);
  table.add ("End", false);
  table.add ("Time", false);
  table.add ("Total", false);

  // Each day is rendered separately.
  time_t grand_total = 0;
  Datetime previous;
  for (Datetime day = filter.range.start; day < filter.range.end; day++)
  {
    auto day_range = getFullDay (day);
    time_t daily_total = 0;

    int row = -1;
    for (auto& track : subset (day_range, tracked))
    {
      // Make sure the track only represents one day.
      if ((track.range.is_open () && day > Datetime ()))
        continue;

      row = table.addRow ();

      if (day != previous)
      {
        table.set (row, 0, format ("W{1}", day.week ()));
        table.set (row, 1, day.toString ("Y-M-D"));
        table.set (row, 2, day.dayNameShort (day.dayOfWeek ()));
        previous = day;
      }

      // Intersect track with day.
      auto today = day_range.intersect (track.range);
      if (track.range.is_open () && day <= Datetime ())
        today.end = Datetime ();

      std::string tags = "";
      for (auto& tag : track.tags ())
      {
        if (tags != "")
          tags += ", ";
        tags += tag;
      }

      table.set (row, 3, tags);
      table.set (row, 4, today.start.toString ("h:N:S"));
      table.set (row, 5, (track.range.is_open () ? "-" : today.end.toString ("h:N:S")));
      table.set (row, 6, Duration (today.total ()).formatHours ());

      daily_total += today.total ();
    }

    if (row != -1)
      table.set (row, 7, Duration (daily_total).formatHours ());

    grand_total += daily_total;
  }

  // Add the total.
  table.set (table.addRow (), 7, " ", Color ("underline"));
  table.set (table.addRow (), 7, Duration (grand_total).formatHours ());

  if (table.rows () > 2)
    std::cout << '\n'
              << table.render ()
              << '\n';
  else
    std::cout << "No filtered data found.\n";

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
