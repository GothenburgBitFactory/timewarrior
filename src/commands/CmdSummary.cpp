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

#include <Table.h>
#include <Duration.h>
#include <shared.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>
#include <IntervalFilterAndGroup.h>
#include <IntervalFilterAllWithTags.h>
#include <IntervalFilterAllInRange.h>

// Implemented in CmdChart.cpp.
std::map <Datetime, std::string> createHolidayMap (Rules&, Interval&);
std::string renderHolidays (const std::map <Datetime, std::string>&);

////////////////////////////////////////////////////////////////////////////////
int CmdSummary (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  const bool verbose = rules.getBoolean ("verbose");

  // Create a filter, and if empty, choose 'today'.
  auto default_hint = rules.get ("reports.range", ":day");
  auto report_hint = rules.get ("reports.summary.range", default_hint);

  Range default_range = {};
  expandIntervalHint (report_hint, default_range);

  auto filter = cli.getFilter (default_range);

  // Load the data.
  IntervalFilterAndGroup filtering ({
    std::make_shared <IntervalFilterAllInRange> ( Range { filter.start, filter.end }),
    std::make_shared <IntervalFilterAllWithTags> (filter.tags())
  });

  auto tracked = getTracked (database, rules, filtering);

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
  Color colorID (rules.getBoolean ("color") ? rules.get ("theme.colors.ids") : "");

  const auto show_ids = cli.getComplementaryHint ("ids", rules.getBoolean ("reports.summary.ids"));
  const auto show_annotations = cli.getComplementaryHint ("annotations", rules.getBoolean ("reports.summary.annotations"));

  Table table;
  table.width (1024);
  table.colorHeader (Color ("underline"));
  table.add ("Wk");
  table.add ("Date");
  table.add ("Day");

  if (show_ids)
  {
    table.add ("ID");
  }

  table.add ("Tags");

  auto offset = 0;

  if (show_annotations)
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

  const auto now = Datetime ();

  if (days_end == 0)
  {
    days_end = now;
  }

  for (Datetime day = days_start.startOfDay (); day < days_end; ++day)
  {
    auto day_range = getFullDay (day);
    time_t daily_total = 0;

    int row = -1;
    for (auto& track : subset (day_range, tracked))
    {
      // Make sure the track only represents one day.
      if ((track.is_open () && day > now))
      {
        continue;
      }

      row = table.addRow ();

      if (day != previous)
      {
        table.set (row, 0, format ("W{1}", day.week ()));
        table.set (row, 1, day.toString ("Y-M-D"));
        table.set (row, 2, Datetime::dayNameShort (day.dayOfWeek ()));
        previous = day;
      }


      // Intersect track with day.
      auto today = day_range.intersect (track);

      if (track.is_open() && track.start > now)
      {
        today.end = track.start;
      }
      else if (track.is_open () && day <= now && today.end > now)
      {
        today.end = now;
      }

      std::string tags = join(", ", track.tags());

      if (show_ids)
      {
        table.set (row, 3, format ("@{1}", track.id), colorID);
      }

      table.set (row, 3 + (show_ids ? 1 : 0), tags, summaryIntervalColor (rules, track.tags ()));

      if (show_annotations)
      {
        auto annotation = track.getAnnotation ();

        if (annotation.length () > 15)
        {
          annotation = annotation.substr (0, 12) + "...";
        }

        table.set (row, 4 + (show_ids ? 1 : 0), annotation);
      }

      const auto total = today.total ();
      
      table.set (row, 4 + (show_ids ? 1 : 0) + offset, today.start.toString ("h:N:S"));
      table.set (row, 5 + (show_ids ? 1 : 0) + offset, (track.is_open () ? "-" : today.end.toString ("h:N:S")));
      table.set (row, 6 + (show_ids ? 1 : 0) + offset, Duration (total).formatHours ());

      daily_total += total;
    }

    if (row != -1)
      table.set (row, 7 + (show_ids ? 1 : 0) + offset, Duration (daily_total).formatHours ());

    grand_total += daily_total;
  }

  // Add the total.
  table.set (table.addRow (), 7 + (show_ids ? 1 : 0) + offset, " ", Color ("underline"));
  table.set (table.addRow (), 7 + (show_ids ? 1 : 0) + offset, Duration (grand_total).formatHours ());

  const auto show_holidays = cli.getComplementaryHint ("holidays", rules.getBoolean ("reports.summary.holidays"));

  std::cout << '\n'
            << table.render ()
            << (show_holidays ? renderHolidays (createHolidayMap (rules, filter)) : "")
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
