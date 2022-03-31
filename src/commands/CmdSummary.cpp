////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2022, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <IntervalFilterAllInRange.h>
#include <IntervalFilterAllWithTags.h>
#include <IntervalFilterAndGroup.h>
#include <Table.h>
#include <commands.h>
#include <format.h>
#include <iostream>
#include <timew.h>
#include <utf8.h>

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

  auto default_hint = rules.get ("reports.range", "day");
  auto report_hint = rules.get ("reports.summary.range", default_hint);

  Range default_range = {};
  expandIntervalHint (":" + report_hint, default_range);

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

  const auto week_fmt = "W{1}";
  const auto date_fmt = "Y-M-D";
  const auto time_fmt = "h:N:S";

  const auto show_weeks = rules.getBoolean ("reports.summary.weeks", true);
  const auto show_weekdays = rules.getBoolean ("reports.summary.weekdays", true);
  const auto show_ids = cli.getComplementaryHint ("ids", rules.getBoolean ("reports.summary.ids"));
  const auto show_tags = cli.getComplementaryHint ("tags", rules.getBoolean ("reports.summary.tags", true));
  const auto show_annotations = cli.getComplementaryHint ("annotations", rules.getBoolean ("reports.summary.annotations"));
  const auto show_holidays = cli.getComplementaryHint ("holidays", rules.getBoolean ("reports.summary.holidays"));

  const auto dates_col_offset = show_weeks ? 1 : 0;
  const auto weekdays_col_offset = dates_col_offset;
  const auto ids_col_offset = weekdays_col_offset + (show_weekdays ? 1: 0);
  const auto tags_col_offset = ids_col_offset + (show_ids ? 1 : 0);
  const auto annotation_col_offset = tags_col_offset + (show_tags ? 1 : 0);
  const auto start_col_offset = annotation_col_offset + (show_annotations ? 1 : 0);

  const auto weeks_col_index = 0;
  const auto dates_col_index = 0 + dates_col_offset;
  const auto weekdays_col_index = 1 + weekdays_col_offset;
  const auto ids_col_index = 1 + ids_col_offset;
  const auto tags_col_index = 1 + tags_col_offset;
  const auto annotation_col_index = 1 + annotation_col_offset;
  const auto start_col_index = 1 + start_col_offset;
  const auto end_col_index = 2 + start_col_offset;
  const auto duration_col_index = 3 + start_col_offset;
  const auto total_col_index = 4 + start_col_offset;

  Table table;
  table.width (1024);
  table.colorHeader (Color ("underline"));

  if (show_weeks)
  {
    table.add ("Wk");
  }

  table.add ("Date");

  if (show_weekdays)
  {
    table.add ("Day");
  }

  if (show_ids)
  {
    table.add ("ID");
  }

  if (show_tags)
  {
    table.add ("Tags");
  }

  if (show_annotations)
  {
    table.add ("Annotation");
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
        if (show_weeks)
        {
          table.set (row, weeks_col_index, format (week_fmt, day.week ()));
        }

        table.set (row, dates_col_index, day.toString (date_fmt));

        if (show_weekdays)
        {
          table.set (row, weekdays_col_index, Datetime::dayNameShort (day.dayOfWeek ()));
        }

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

      if (show_ids)
      {
        table.set (row, ids_col_index, format ("@{1}", track.id), colorID);
      }

      if (show_tags)
      {
        std::string tags = join (", ", track.tags ());
        table.set (row, tags_col_index, tags, summaryIntervalColor (rules, track.tags ()));
      }

      if (show_annotations)
      {
        auto annotation = track.getAnnotation ();

        if (utf8_length (annotation) > 15)
        {
          annotation = utf8_substr (annotation, 0, 12) + "...";
        }

        table.set (row, annotation_col_index, annotation);
      }

      const auto total = today.total ();

      table.set (row, start_col_index, today.start.toString (time_fmt));
      table.set (row, end_col_index, (track.is_open () ? "-" : today.end.toString (time_fmt)));
      table.set (row, duration_col_index, Duration (total).formatHours ());

      daily_total += total;
    }

    if (row != -1)
    {
      table.set (row, total_col_index, Duration (daily_total).formatHours ());
    }

    grand_total += daily_total;
  }

  // Add the total.
  table.set (table.addRow (), total_col_index, " ", Color ("underline"));
  table.set (table.addRow (), total_col_index, Duration (grand_total).formatHours ());

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
