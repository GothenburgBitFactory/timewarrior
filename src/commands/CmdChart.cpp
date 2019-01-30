////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <commands.h>
#include <timew.h>
#include <shared.h>
#include <format.h>
#include <iostream>
#include <iomanip>

int renderChart (const CLI&, const std::string&, Interval&, Rules&, Database&);

std::map <Datetime, std::string> createHolidayMap (Rules&, Interval&);

////////////////////////////////////////////////////////////////////////////////
int CmdChartDay (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose the current day.
  auto filter = getFilter (cli);
  if (! filter.is_started ())
  {
    if (rules.has ("reports.day.range"))
      expandIntervalHint (rules.get ("reports.day.range"), filter);
    else
      filter.setRange (Datetime ("today"), Datetime ("tomorrow"));
  }

  return renderChart (cli, "day", filter, rules, database);
}

////////////////////////////////////////////////////////////////////////////////
int CmdChartWeek (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose the current week.
  auto filter = getFilter (cli);
  if (! filter.is_started ())
  {
    if (rules.has ("reports.week.range"))
      expandIntervalHint (rules.get ("reports.week.range"), filter);
    else
      filter.setRange (Datetime ("sow"), Datetime ("eow"));
  }

  return renderChart (cli, "week", filter, rules, database);
}

////////////////////////////////////////////////////////////////////////////////
int CmdChartMonth (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose the current month.
  auto filter = getFilter (cli);
  if (! filter.is_started ())
  {
    if (rules.has ("reports.month.range"))
      expandIntervalHint (rules.get ("reports.month.range"), filter);
    else
      filter.setRange (Datetime ("som"), Datetime ("eom"));
  }

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
  // Load the data.
  auto exclusions = getAllExclusions (rules, filter);
  auto tracked    = getTracked (database, rules, filter);
  const auto holidays = createHolidayMap (rules, filter);

  // Map tags to colors.
  auto palette = createPalette (rules);
  auto tag_colors = createTagColorMap (rules, palette, tracked);
  auto with_colors = rules.getBoolean ("color");

  Color color_today (with_colors ? rules.get ("theme.colors.today") : "");
  Color color_holiday (with_colors ? rules.get ("theme.colors.holiday") : "");
  Color color_label (with_colors ? rules.get ("theme.colors.label") : "");
  Color color_exclusion (with_colors ? rules.get ("theme.colors.exclusion") : "");

  const auto determine_hour_range = rules.get ("reports." + type + ".hours") == "auto";

  const bool show_intervals = findHint (cli, ":blank");
  const bool with_ids = findHint (cli, ":ids");

  const auto with_summary = rules.getBoolean ("reports." + type + ".summary");
  const auto with_holidays = rules.getBoolean ("reports." + type + ".holidays");
  const auto with_totals = rules.getBoolean ("reports." + type + ".totals");
  const auto with_month = rules.getBoolean ("reports." + type + ".month");
  const auto with_week = rules.getBoolean ("reports." + type + ".week");
  const auto with_day = rules.getBoolean ("reports." + type + ".day");
  const auto with_weekday = rules.getBoolean ("reports." + type + ".weekday");

  const auto minutes_per_char = rules.getInteger ("reports." + type + ".cell");

  if (minutes_per_char < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

  const auto spacing = rules.getInteger ("reports." + type + ".spacing", 1);
  const auto num_lines = rules.getInteger ("reports." + type + ".lines", 1);

  if (num_lines < 1)
    throw format ("Invalid value for 'reports.{1}.lines': '{2}'", type, num_lines);

  auto axis_type = rules.get ("reports." + type + ".axis");
  const auto with_internal_axis = axis_type == "internal";

  Chart chart(with_month, with_week, with_weekday, with_day);

  std::cout << chart.render (filter, tracked, exclusions, holidays, tag_colors, color_today, color_holiday, color_label, color_exclusion, show_intervals, determine_hour_range, with_ids, with_summary, with_holidays, with_totals, with_internal_axis, minutes_per_char, spacing, num_lines);

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
