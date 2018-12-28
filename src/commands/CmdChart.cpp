////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <Composite.h>
#include <Duration.h>
#include <Range.h>
#include <commands.h>
#include <timew.h>
#include <shared.h>
#include <format.h>
#include <iostream>
#include <iomanip>
#include <cassert>

int                renderChart           (const CLI&, const std::string&, Interval&, Rules&, Database&);
static std::pair <int, int> determineHourRange (const Interval&, const std::vector <Interval>&);
static void        renderAxis            (const std::string&, int, int, bool, const Color&, const Color&, int);
static std::string renderMonth           (const Datetime&, const Datetime&);
static std::string renderWeek            (const Datetime&, const Datetime&);
static std::string renderWeekday         (const Rules&, Datetime&, Color&, Color&);
static std::string renderDay             (const Rules&, Datetime&, Color&, Color&);
static std::string renderTotal           (time_t);
static std::string renderSubTotal        (time_t, const unsigned long);
static void        renderExclusionBlocks (const std::string&, const Rules&, std::vector <Composite>&, bool, const Datetime&, int, int, const std::vector <Range>&);
static void        renderInterval        (const std::string&, const Rules&, std::vector <Composite>&, const Datetime&, const Interval&, std::map <std::string, Color>&, int, time_t&, bool);
       std::string renderHolidays        (const Rules&, const Interval&, const std::vector<std::string>&);
static std::string renderSummary         (const std::string&, const Interval&, const std::vector <Range>&, const std::vector <Interval>&, bool);

unsigned long getIndentSize (const std::string &type, const Rules &rules);

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

  // Map tags to colors.
  auto palette = createPalette (rules);
  auto tag_colors = createTagColorMap (rules, palette, tracked);
  auto with_colors = rules.getBoolean ("color");
  
  Color colorToday (with_colors ? rules.get ("theme.colors.today") : "");
  Color colorHoliday (with_colors ? rules.get ("theme.colors.holiday") : "");

  const auto not_full_day = rules.get ("reports." + type + ".hours") == "auto";

  // Determine hours shown.
  auto hour_range = not_full_day
    ? determineHourRange (filter, tracked)
    : std::make_pair (0, 23);

  int first_hour = hour_range.first;
  int last_hour = hour_range.second;

  debug (format ("Day range is from {1}:00 - {2}:00", first_hour, last_hour));

  const auto indent_size = getIndentSize (type, rules);
  auto indent = std::string (indent_size, ' ');

  // Is the :blank hint being used?
  bool blank = findHint (cli, ":blank");
  bool ids   = findHint (cli, ":ids");

  const auto with_summary = rules.getBoolean ("reports." + type + ".summary");
  const auto with_holidays = rules.getBoolean ("reports." + type + ".holidays");
  const auto with_totals = rules.getBoolean ("reports." + type + ".totals");
  const auto with_month = rules.getBoolean ("reports." + type + ".month");
  const auto with_week = rules.getBoolean ("reports." + type + ".week");
  const auto with_day = rules.getBoolean ("reports." + type + ".day");
  const auto with_weekday = rules.getBoolean ("reports." + type + ".weekday");

  // Determine how much space is occupied by the left-margin labels.
  const auto minutes_per_char = rules.getInteger ("reports." + type + ".cell");
  if (minutes_per_char < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

  const auto spacing = rules.getInteger ("reports." + type + ".spacing", 1);
  const auto num_lines = rules.getInteger ("reports." + type + ".lines", 1);

  if (num_lines < 1)
    throw format ("Invalid value for 'reports.{1}.lines': '{2}'", type, rules.get ("reports." + type + ".lines"));

  auto color_today = Color (with_colors ? rules.get ("theme.colors.today") : "");
  auto color_label = Color (with_colors ? rules.get ("theme.colors.label") : "");

  const auto chars_per_hour = 60 / minutes_per_char;
  const auto cell_size = chars_per_hour + spacing;
  const auto padding_size = indent_size + ((last_hour - first_hour + 1) * (cell_size)) + 1;

  auto axis_type = rules.get ("reports." + type + ".axis");

  // Render the axis.
  std::cout << '\n';

  if (axis_type != "internal")
  {
    renderAxis (
      indent,
      first_hour,
      last_hour,
      with_totals,
      color_label,
      color_today,
      cell_size);
  }

  // For rendering labels on edge detection.
  Datetime previous {0};


  // Each day is rendered separately.
  time_t total_work = 0;
  for (Datetime day = filter.start; day < filter.end; day++)
  {
    // Render the exclusion blocks.

    // Add an empty string with no color, to reserve width, so this function
    // can simply concatenate to lines[i].str ().
    int total_width = (last_hour - first_hour + 1) * (chars_per_hour + spacing) - 1;
    std::vector <Composite> lines (num_lines);
    for (int i = 0; i < num_lines; ++i)
      lines[i].add (std::string (total_width, ' '), 0, Color ());

    renderExclusionBlocks (type, rules, lines, with_colors, day, first_hour, last_hour, exclusions);

    time_t work = 0;
    if (! blank)
    {
      for (auto& track : tracked)
      {
        time_t interval_work = 0;
        renderInterval (type, rules, lines, day, track, tag_colors, first_hour, interval_work, ids);
        work += interval_work;
      }
    }

    auto labelMonth   = with_month ? renderMonth (previous, day) : "";
    auto labelWeek    = with_week ? renderWeek (previous, day) : "";
    auto labelWeekday = with_weekday ? renderWeekday (rules, day, colorToday, colorHoliday) : "";
    auto labelDay     = with_day ? renderDay (rules, day, colorToday, colorHoliday) : "";

    std::cout << labelMonth
              << labelWeek
              << labelWeekday
              << labelDay
              << lines[0].str ();

    if (lines.size () > 1)
      for (unsigned int i = 1; i < lines.size (); ++i)
        std::cout << "\n"
                  << indent
                  << lines[i].str ();

    std::cout << (with_totals ? renderTotal (work) : "")
              << '\n';

    previous = day;
    total_work += work;
  }

  std::cout << (with_totals ? renderSubTotal (total_work, padding_size) : "")
            << (with_holidays ? renderHolidays (rules, filter, rules.all ("holidays.")) : "")
            << (with_summary ? renderSummary (indent, filter, exclusions, tracked, blank) : "");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
unsigned long getIndentSize (const std::string &type, const Rules &rules)
{
  return (rules.getBoolean ("reports." + type + ".month")   ? 4 : 0) +
         (rules.getBoolean ("reports." + type + ".week")    ? 4 : 0) +
         (rules.getBoolean ("reports." + type + ".day")     ? 3 : 0) +
         (rules.getBoolean ("reports." + type + ".weekday") ? 4 : 0);
}

////////////////////////////////////////////////////////////////////////////////
// Scan all tracked intervals, looking for the earliest and latest hour into
// which an interval extends.
static std::pair <int, int> determineHourRange (
  const Interval& filter,
  const std::vector <Interval>& tracked)
{
  // Default to the full day.
  auto first_hour = 0;
  auto last_hour = 23;

  // If there is no data,
  // show the whole day.
  if (! tracked.empty ())
  {
    // Get the extreme time range for the filtered data.
    first_hour = 23;
    last_hour  = 0;

    for (Datetime day = filter.start; day < filter.end; day++)
    {
      auto day_range = getFullDay (day);

      for (auto& track : tracked)
      {
        if (day_range.overlaps (track))
        {
          Interval clipped = clip (track, day_range);
          if (track.is_open ())
          {
            clipped.end = Datetime ();
          }

          if (clipped.start.hour () < first_hour)
          {
            first_hour = clipped.start.hour ();
          }

          if (! clipped.is_open () && clipped.end.hour () > last_hour)
          {
            last_hour = clipped.end.hour ();
          }
        }
      }
    }

    if (first_hour == 23 && last_hour == 0)
    {
      first_hour = Datetime ().hour ();
      last_hour = std::min (first_hour + 1, 23);
    }
    else
    {
      first_hour = std::max (first_hour - 1, 0);
      last_hour  = std::min (last_hour + 1, 23);
    }
  }

  return std::make_pair (first_hour, last_hour);
}

////////////////////////////////////////////////////////////////////////////////
static void renderAxis (
  const std::string& indent,
  int first_hour,
  int last_hour,
  bool with_totals,
  const Color& colorLabel,
  const Color& colorToday,
  const int cell_size)
{
  auto current_hour = Datetime ().hour ();

  std::cout << indent;
  for (int hour = first_hour; hour <= last_hour; hour++)
  {
    if (hour == current_hour)
    {
      std::cout << colorToday.colorize (leftJustify (hour, cell_size));
    }
    else
    {
      std::cout << colorLabel.colorize (leftJustify (hour, cell_size));
    }
  }

  if (with_totals)
  {
    std::cout << "  " << colorLabel.colorize ("Total");
  }

  std::cout << '\n';
}

////////////////////////////////////////////////////////////////////////////////
static std::string renderMonth (const Datetime &previous, const Datetime &day)
{
  const auto show_month = previous.month () != day.month ();

  std::stringstream out;

  out << (show_month ? Datetime::monthNameShort (day.month ()) : "   ")
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Includes trailing separator space.
static std::string renderWeek (const Datetime &previous, const Datetime &day)
{
  const auto show_week = previous.week () != day.week ();

  std::stringstream out;

  out << (show_week ? leftJustify (format ("W{1}", day.week ()), 3) : "   ")
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Today should be highlighted.
// Includes trailing separator space.
static std::string renderWeekday (
  const Rules& rules,
  Datetime& day,
  Color& colorToday,
  Color& colorHoliday)
{
  Color color;
  if (day.sameDay (Datetime ()))
  {
    color = colorToday;
  }
  else if (dayIsHoliday (rules, day))
  {
    color = colorHoliday;
  }

  std::stringstream out;
  out << color.colorize (Datetime::dayNameShort (day.dayOfWeek ()))
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static std::string renderDay (
  const Rules& rules,
  Datetime& day,
  Color& colorToday,
  Color& colorHoliday)
{
  Color color;

  if (day.sameDay (Datetime ()))
  {
    color = colorToday;
  }
  else if (dayIsHoliday (rules, day))
  {
    color = colorHoliday;
  }

  std::stringstream out;

  out << color.colorize (rightJustify (day.day (), 2))
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static std::string renderTotal (time_t work)
{
  std::stringstream out;

  if (work)
  {
    int hours = work / 3600;
    int minutes = (work % 3600) / 60;
    std::cout << " "
              << std::setw (3) << std::setfill (' ') << hours
              << ':'
              << std::setw (2) << std::setfill ('0') << minutes;
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static std::string renderSubTotal (
  time_t total_work,
  const unsigned long padding_size)
{
  std::stringstream out;

  std::string pad (padding_size, ' ');

  int hours = total_work / 3600;
  int minutes = (total_work % 3600) / 60;

  out << pad
      << Color ("underline").colorize ("      ")
      << '\n'
      << pad
      << std::setw (3) << std::setfill (' ') << hours
      << ':'
      << std::setw (2) << std::setfill ('0') << minutes
      << '\n';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static void renderExclusionBlocks (
  const std::string& type,
  const Rules& rules,
  std::vector <Composite>& lines,
  const bool with_colors,
  const Datetime& day,
  int first_hour,
  int last_hour,
  const std::vector <Range>& excluded)
{
  auto minutes_per_char = rules.getInteger ("reports." + type + ".cell");
  if (minutes_per_char < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

  auto chars_per_hour = 60 / minutes_per_char;

  auto spacing = rules.getInteger ("reports." + type + ".spacing");
  auto axis_type = rules.get ("reports." + type + ".axis");
  const auto cell_width = chars_per_hour + spacing;
  Color colorExc (with_colors ? rules.get ("theme.colors.exclusion") : "");
  Color colorLabel (with_colors ? rules.get ("theme.colors.label") : "");

  // Render the exclusion blocks.
  for (int hour = first_hour; hour <= last_hour; hour++)
  {
    // Construct a range representing a single 'hour', of 'day'.
    Range r (Datetime (day.year (), day.month (), day.day (), hour, 0, 0),
             Datetime (day.year (), day.month (), day.day (), hour + 1, 0, 0));

    if (axis_type == "internal")
    {
      auto label = format ("{1}", hour);
      int offset = (hour - first_hour) * cell_width;
      lines[0].add (label, offset, colorLabel);
    }

    for (auto& exc : excluded)
    {
      if (exc.overlaps (r))
      {
        // Determine which of the character blocks included.
        auto sub_hour = exc.intersect (r);
        auto start_block = quantizeToNMinutes (sub_hour.start.minute (), minutes_per_char) / minutes_per_char;
        auto end_block   = quantizeToNMinutes (sub_hour.end.minute () == 0 ? 60 : sub_hour.end.minute (), minutes_per_char) / minutes_per_char;

        int offset = (hour - first_hour) * cell_width + start_block;
        int width = end_block - start_block;
        std::string block (width, ' ');

        for (auto& line : lines)
          line.add (block, offset, colorExc);

        if (axis_type == "internal")
        {
          auto label = format ("{1}", hour);
          if (start_block == 0 &&
              width >= static_cast <int> (label.length ()))
            lines[0].add (label, offset, colorExc);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderInterval (
  const std::string& type,
  const Rules& rules,
  std::vector <Composite>& lines,
  const Datetime& day,
  const Interval& track,
  std::map <std::string, Color>& tag_colors,
  int first_hour,
  time_t& work,
  bool ids)
{
  Datetime now;
  auto cell = rules.getInteger ("reports." + type + ".cell");
  if (cell < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);
  auto spacing = rules.getInteger ("reports." + type + ".spacing");

  // Ignore any track that doesn't overlap with day.
  auto day_range = getFullDay (day);
  if (! day_range.overlaps (track) || (track.is_open () && day > now))
  {
    return;
  }

  // If the track is open and day is today, then closed the track now, otherwise
  // it will be rendered until midnight.
  Interval clipped = clip (track, day_range);
  if (track.is_open ())
  {
    if (day_range.start.sameDay (now))
    {
      clipped.end = now;
    }
    else
    {
      clipped.end = day_range.end;
    }
  }

  auto start_mins = (clipped.start.hour () - first_hour) * 60 + clipped.start.minute ();
  auto end_mins   = (clipped.end.hour ()   - first_hour) * 60 + clipped.end.minute ();

  if (clipped.end.hour () == 0)
  {
    end_mins += (clipped.end.day() + (clipped.end.month () - clipped.start.month () - 1) * clipped.start.day ()) * 24 * 60;
  }

  work = clipped.total ();

  auto start_block = quantizeToNMinutes (start_mins, cell) / cell;
  auto end_block   = quantizeToNMinutes (end_mins == start_mins ? start_mins + 60 : end_mins, cell) / cell;

  int start_offset = start_block + (spacing * (start_mins / 60));
  int end_offset   = end_block   + (spacing * (end_mins   / 60));

  if (end_offset > start_offset)
  {
    // Determine color of interval.
    Color colorTrack = intervalColor (track, rules, tag_colors);

    // Properly format the tags within the space.
    std::string label;
    if (ids)
    {
      label = format ("@{1}", track.id);
    }

    for (auto& tag : track.tags ())
    {
      if (! label.empty ())
      {
        label += ' ';
      }

      label += tag;
    }

    auto width = end_offset - start_offset;
    if (width)
    {
      std::vector <std::string> text_lines;
      wrapText (text_lines, label, width, false);

      for (unsigned int i = 0; i < lines.size (); ++i)
      {
        if (i < text_lines.size ())
        {
          lines[i].add (leftJustify (text_lines[i], width), start_offset, colorTrack);
        }
        else
        {
          lines[i].add (std::string (width, ' '), start_offset, colorTrack);
        }
      }

      // An open interval gets a "+" in the bottom right corner
      if (track.is_open ())
      {
        lines.back ().add ("+", start_offset + width - 1, colorTrack);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string renderHolidays (
  const Rules& rules,
  const Interval& filter,
  const std::vector <std::string>& holidays)
{
  std::stringstream out;

  for (auto& entry : holidays)
  {
    auto first_dot = entry.find ('.');
    auto last_dot = entry.rfind ('.');

    if (last_dot != std::string::npos)
    {
      auto date = entry.substr (last_dot + 1);
      std::replace (date.begin (), date.end (), '_', '-');
      Datetime holiday (date);

      if (holiday >= filter.start &&
          holiday <= filter.end)
      {
        out << Datetime (date).toString ("Y-M-D")
            << " ["
            << entry.substr (first_dot + 1, last_dot - first_dot - 1)
            << "] "
            << rules.get (entry)
            << '\n';
      }
    }
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static std::string renderSummary (
  const std::string& indent,
  const Interval& filter,
  const std::vector <Range>& exclusions,
  const std::vector <Interval>& tracked,
  bool blank)
{
  std::stringstream out;
  time_t total_unavailable = 0;

  for (auto& exclusion : exclusions)
  {
    if (filter.overlaps (exclusion))
    {
      total_unavailable += filter.intersect (exclusion).total ();
    }
  }

  time_t total_worked = 0;

  if (! blank)
  {
    for (auto& interval : tracked)
    {
      if (filter.overlaps (interval))
      {
        Interval clipped = clip (interval, filter);
        if (interval.is_open ())
        {
          clipped.end = Datetime ();
        }

        total_worked += clipped.total ();
      }
    }
  }

  auto total_available = filter.total () - total_unavailable;
  assert (total_available >= 0);
  auto total_remaining = total_available - total_worked;

  out << '\n'
      << indent << "Tracked   "
      << std::setw (13) << std::setfill (' ') << Duration (total_worked).formatHours ()    << '\n';

  if (total_remaining >= 0)
  {
    out << indent << "Available "
        << std::setw (13) << std::setfill (' ') << Duration (total_remaining).formatHours () << '\n';
  }

  out << indent << "Total     "
      << std::setw (13) << std::setfill (' ') << Duration (total_available).formatHours ()   << '\n'
      << '\n';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
