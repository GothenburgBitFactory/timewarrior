////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <IntervalFilterAllWithIds.h>
#include <IntervalFilterAllWithTags.h>
#include <IntervalFilterAndGroup.h>
#include <SummaryTable.h>
#include <Table.h>
#include <commands.h>
#include <format.h>
#include <iostream>
#include <timew.h>
#include <utf8.h>

// Implemented in CmdChart.cpp.
std::map <Datetime, std::string> createHolidayMap (Rules&, Range&);
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

  auto ids = cli.getIds ();
  auto tags = cli.getTags ();

  if (! ids.empty () && ! tags.empty ())
  {
    throw std::string ("You cannot filter intervals by both, ids and tags.");
  }

  Range range;

  std::vector <Interval> tracked;

  if (! ids.empty ())
  {
    auto filtering = IntervalFilterAllWithIds (ids);
    tracked = getTracked (database, rules, filtering);

    if (tracked.size () != ids.size ())
    {
      for (auto& id: ids)
      {
        bool found = false;

        for (auto& interval: tracked)
        {
          if (interval.id == id)
          {
            found = true;
            break;
          }
        }
        if (! found)
        {
          throw format ("ID '@{1}' does not correspond to any tracking.", id);
        }
      }
    }

    range = Range {tracked.begin ()->end, tracked.end ()->start};
  }
  else
  {
    range = cli.getRange (default_range);

    IntervalFilterAndGroup filtering ({
      std::make_shared <IntervalFilterAllInRange> (range),
      std::make_shared <IntervalFilterAllWithTags> (tags)
    });

    tracked = getTracked (database, rules, filtering);
  }

  if (tracked.empty ())
  {
    if (verbose)
    {
      std::cout << "No filtered data found";

      if (range.is_started ())
      {
        std::cout << " in the range " << range.start.toISOLocalExtended ();
        if (range.is_ended ())
        {
          std::cout << " - " << range.end.toISOLocalExtended ();
        }
      }

      if (! tags.empty ())
      {
        std::cout << " tagged with " << joinQuotedIfNeeded (", ", tags);
      }

      std::cout << ".\n";
    }

    return 0;
  }

  // Map tags to colors.
  Color colorID (rules.getBoolean ("color") ? rules.get ("theme.colors.ids") : "");
  auto tagColorMap = createTagColorMap (rules, tracked);

  const auto show_weeks = rules.getBoolean ("reports.summary.weeks", true);
  const auto show_weekdays = rules.getBoolean ("reports.summary.weekdays", true);
  const auto show_ids = cli.getComplementaryHint ("ids", rules.getBoolean ("reports.summary.ids"));
  const auto show_tags = cli.getComplementaryHint ("tags", rules.getBoolean ("reports.summary.tags", true));
  const auto show_annotations = cli.getComplementaryHint ("annotations", rules.getBoolean ("reports.summary.annotations"));
  const auto show_holidays = cli.getComplementaryHint ("holidays", rules.getBoolean ("reports.summary.holidays"));

  auto table = SummaryTable::builder ()
    .withWeekFormat ("W{1}")
    .withDateFormat ("Y-M-D")
    .withTimeFormat ("h:N:S")
    .withWeeks (show_weeks)
    .withWeekdays (show_weekdays)
    .withIds (show_ids, colorID)
    .withTags (show_tags, tagColorMap)
    .withAnnotations (show_annotations)
    .withRange (range)
    .withIntervals (tracked)
    .build ();

  std::cout << '\n'
            << table.render ()
            << (show_holidays ? renderHolidays (createHolidayMap (rules, range)) : "")
            << '\n';

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
std::string renderHolidays (const std::map <Datetime, std::string>& holidays)
{
  std::stringstream out;

  for (auto& entry : holidays)
  {
    out << entry.first.toString ("Y-M-D")
        << " "
        << entry.second
        << '\n';
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
