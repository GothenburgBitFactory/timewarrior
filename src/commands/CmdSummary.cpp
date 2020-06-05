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
#include <shared.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

// Implemented in CmdChart.cpp.
std::map <Datetime, std::string> createHolidayMap (Rules&, Interval&);
std::string renderHolidays (const std::map <Datetime, std::string>&);

////////////////////////////////////////////////////////////////////////////////
int CmdSummary (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  auto verbose = rules.getBoolean ("verbose");

  // Create a filter, and if empty, choose 'today'.
  auto filter = cli.getFilter (Range { Datetime ("today"), Datetime ("tomorrow") });

  // Load the data.
  auto tracked = getTracked (database, rules, filter);

  if (tracked.empty ())
  {
    if (verbose)
    {
      std::cout << "No filtered data found";

      if (filter.is_started ())
      {
        std::cout << " in the range " << filter.start.toISOLocalExtended ();
        if (filter.is_ended ())
          std::cout << " - " << filter.end.toISOLocalExtended ();
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
  auto show_annotation = findHint (cli, ":annotations");

  Table table;
  table.width (1024);
  table.colorHeader (Color ("underline"));
  table.add ("Wk");
  table.add ("Date");
  table.add ("Day");

  if (ids)
  {
    table.add ("ID");
  }

  table.add ("Tags");

  auto offset = 0;

  if (show_annotation)
  {
    table.add ("Annotation");
    offset = 1;
  }

  table.add ("Start", false);
  table.add ("End", false);
  table.add ("Time", false);
  table.add ("Total", false);

  // Each day is rendered separately.
  time_t grand_total = 0;
  Datetime previous;

  auto days_start = filter.is_started() ? filter.start : tracked.front ().start;
  auto days_end   = filter.is_ended()   ? filter.end   : tracked.back ().end;

  for (Datetime day = days_start; day < days_end; day++)
  {
    auto day_range = getFullDay (day);
    time_t daily_total = 0;

    int row = -1;
    for (auto& track : subset (day_range, tracked))
    {
      // Make sure the track only represents one day.
      if ((track.is_open () && day > Datetime ()))
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
      auto today = day_range.intersect (track);
      if (track.is_open () && day <= Datetime () && today.end > Datetime ())
        today.end = Datetime ();

      std::string tags = join(", ", track.tags());

      if (ids)
      {
        table.set (row, 3, format ("@{1}", track.id), colorID);
      }

      table.set (row, (ids ? 4 : 3), tags);

      if (show_annotation)
      {
        auto annotation = track.getAnnotation ();

        if (annotation.length () > 15)
          annotation = annotation.substr (0, 12) + "...";

        table.set (row, (ids ? 5 : 4), annotation);
      }

      table.set (row, (ids ? 5 : 4) + offset, today.start.toString ("h:N:S"));
      table.set (row, (ids ? 6 : 5) + offset, (track.is_open () ? "-" : today.end.toString ("h:N:S")));
      table.set (row, (ids ? 7 : 6) + offset, Duration (today.total ()).formatHours ());

      daily_total += today.total ();
    }

    if (row != -1)
      table.set (row, (ids ? 8 : 7) + offset, Duration (daily_total).formatHours ());

    grand_total += daily_total;
  }

  // Add the total.
  table.set (table.addRow (), (ids ? 8 : 7) + offset, " ", Color ("underline"));
  table.set (table.addRow (), (ids ? 8 : 7) + offset, Duration (grand_total).formatHours ());

  const auto with_holidays = rules.getBoolean ("reports.summary.holidays");

  std::cout << '\n'
            << table.render ()
            << (with_holidays ? renderHolidays (createHolidayMap (rules, filter)) : "")
            << '\n';

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string renderHolidays (const std::map<Datetime, std::string> &holidays)
{
  std::stringstream out;

  for (auto &entry : holidays)
  {
    out << entry.first.toString ("Y-M-D")
        << " "
        << entry.second
        << '\n';
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
