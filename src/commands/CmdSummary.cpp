////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Paul Beckingham, Federico Hernandez.
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
#include <shared.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

// Implemented in CmdChart.cpp.
std::string renderHolidays (const std::string&, const Rules&, const Interval&);

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

  if (! filter.range.is_ended())
    filter.range.end = filter.range.start + Duration("1d").toTime_t();

  // Load the data.
  auto tracked = getTracked (database, rules, filter);

  if (tracked.empty ())
  {
    if (rules.getBoolean ("verbose"))
    {
      std::cout << "No filtered data found";

      if (filter.range.is_started ())
      {
        std::cout << " in the range " << filter.range.start.toISOLocalExtended ();
        if (filter.range.is_ended ())
          std::cout << " - " << filter.range.end.toISOLocalExtended ();
      }

      if (! filter.tags ().empty ())
      {
        std::cout << " tagged with " << joinQuotedIfNeeded (", ", filter.tags ());
      }

      std::cout << ".\n";
    }

    return 0;
  }

  // Map tags to colors.
  auto palette = createPalette (rules);
  auto tag_colors = createTagColorMap (rules, palette, tracked);
  Color colorID (rules.getBoolean ("color") ? rules.get ("theme.colors.ids") : "");

  auto ids = findHint (cli, ":ids");

  Table table;
  table.width (1024);
  table.colorHeader (Color ("underline"));
  table.add ("Wk");
  table.add ("Date");
  table.add ("Day");

  if (ids)
    table.add ("ID");

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
      if (track.range.is_open () && day <= Datetime () && today.end > Datetime ())
        today.end = Datetime ();

      std::string tags = join(", ", track.tags());

      if (ids)
        table.set (row, 3, format ("@{1}", track.id), colorID);

      table.set (row, (ids ? 4 : 3), tags);
      table.set (row, (ids ? 5 : 4), today.start.toString ("h:N:S"));
      table.set (row, (ids ? 6 : 5), (track.range.is_open () ? "-" : today.end.toString ("h:N:S")));
      table.set (row, (ids ? 7 : 6), Duration (today.total ()).formatHours ());

      daily_total += today.total ();
    }

    if (row != -1)
      table.set (row, (ids ? 8 : 7), Duration (daily_total).formatHours ());

    grand_total += daily_total;
  }

  // Add the total.
  table.set (table.addRow (), (ids ? 8 : 7), " ", Color ("underline"));
  table.set (table.addRow (), (ids ? 8 : 7), Duration (grand_total).formatHours ());

  std::cout << '\n'
            << table.render ()
            << renderHolidays ("summary", rules, filter)
            << '\n';

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
