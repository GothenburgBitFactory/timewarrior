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

#include <Duration.h>
#include <Range.h>
#include <Chart.h>
#include <ChartConfig.h>
#include <commands.h>
#include <timew.h>
#include <shared.h>
#include <format.h>
#include <iostream>
#include <iomanip>
#include <IntervalFilterAllInRange.h>
#include <IntervalFilterAllWithTags.h>
#include <IntervalFilterAndGroup.h>

int renderChart (const CLI&, const std::string&, Interval&, Rules&, Database&);

std::map <Datetime, std::string> createHolidayMap (Rules&, Interval&);

////////////////////////////////////////////////////////////////////////////////
int CmdChartDay (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  Range default_range = {};
  expandIntervalHint (rules.get ("reports.day.range", ":day"), default_range);

  // Create a filter, and if empty, choose the current day.
  auto filter = cli.getFilter (default_range);

  return renderChart (cli, "day", filter, rules, database);
}

////////////////////////////////////////////////////////////////////////////////
int CmdChartWeek (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  Range default_range = {};
  expandIntervalHint (rules.get ("reports.week.range", ":week"), default_range);

  // Create a filter, and if empty, choose the current week.
  auto filter = cli.getFilter (default_range);

  return renderChart (cli, "week", filter, rules, database);
}

////////////////////////////////////////////////////////////////////////////////
int CmdChartMonth (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  Range default_range = {};
  expandIntervalHint (rules.get ("reports.month.range", ":month"), default_range);

  // Create a filter, and if empty, choose the current month.
  auto filter = cli.getFilter (default_range);

  return renderChart (cli, "month", filter, rules, database);
}

////////////////////////////////////////////////////////////////////////////////
int renderChart (
  const CLI& cli,
  const std::string& type,
  Interval& filter,
  Rules& rules,
  Database& database)
{
  const bool verbose = rules.getBoolean ("verbose");

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

  const auto exclusions = getAllExclusions (rules, filter);
  const auto holidays = createHolidayMap (rules, filter);

  // Map tags to colors.
  auto palette = createPalette (rules);
  auto with_colors = rules.getBoolean ("color");

  const auto minutes_per_char = rules.getInteger ("reports." + type + ".cell");

  if (minutes_per_char < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

  const auto num_lines = rules.getInteger ("reports." + type + ".lines", 1);

  if (num_lines < 1)
    throw format ("Invalid value for 'reports.{1}.lines': '{2}'", type, num_lines);

  ChartConfig configuration {};
  configuration.reference_datetime = Datetime ();
  configuration.with_label_month = rules.getBoolean ("reports." + type + ".month");
  configuration.with_label_week = rules.getBoolean ("reports." + type + ".week");
  configuration.with_label_weekday = rules.getBoolean ("reports." + type + ".weekday");
  configuration.with_label_day = rules.getBoolean ("reports." + type + ".day");
  configuration.with_ids = findHint (cli, ":ids");
  configuration.with_summary = rules.getBoolean ("reports." + type + ".summary");
  configuration.with_holidays = rules.getBoolean ("reports." + type + ".holidays");
  configuration.with_totals = rules.getBoolean ("reports." + type + ".totals");
  configuration.with_internal_axis = rules.get ("reports." + type + ".axis") == "internal";
  configuration.show_intervals = findHint (cli, ":blank");
  configuration.determine_hour_range = rules.get ("reports." + type + ".hours") == "auto";
  configuration.minutes_per_char = minutes_per_char;
  configuration.spacing = rules.getInteger ("reports." + type + ".spacing", 1);
  configuration.num_lines = num_lines;
  configuration.color_today = (with_colors ? Color (rules.get ("theme.colors.today")) : Color (""));
  configuration.color_holiday = (with_colors ? Color (rules.get ("theme.colors.holiday")) : Color (""));
  configuration.color_label = (with_colors ? Color (rules.get ("theme.colors.label")) : Color (""));
  configuration.color_exclusion = (with_colors ? Color (rules.get ("theme.colors.exclusion")) : Color (""));
  configuration.tag_colors = createTagColorMap (rules, palette, tracked);

  Chart chart (configuration);

  std::cout << chart.render (filter, tracked, exclusions, holidays);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
std::map <Datetime, std::string> createHolidayMap (Rules &rules, Interval &filter)
{
  std::map <Datetime, std::string> mapping;
  auto holidays = rules.all ("holidays.");

  for (auto &entry : holidays)
  {
    auto first_dot = entry.find ('.');
    auto last_dot = entry.rfind ('.');

    if (last_dot != std::string::npos)
    {
      auto date = entry.substr (last_dot + 1);
      std::replace (date.begin (), date.end (), '_', '-');
      Datetime holiday (date);

      if (holiday >= filter.start && holiday <= filter.end)
      {
        std::stringstream out;
        out << " ["
            << entry.substr (first_dot + 1, last_dot - first_dot - 1)
            << "] "
            << rules.get (entry);
        auto locale = entry.substr (first_dot + 1, last_dot - first_dot - 1);
        mapping[holiday] = out.str ();
      }
    }
  }

  return mapping;
}

////////////////////////////////////////////////////////////////////////////////
