////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
static void        determineHourRange    (const std::string&, const Rules&, const Interval&, const std::vector <Interval>&, int&, int&);
static void        renderAxis            (const std::string&, const Rules&, Palette&, const std::string&, int, int);
static std::string renderMonth           (const std::string&, const Rules&, const Datetime&, const Datetime&);
static std::string renderDayName         (const std::string&, const Rules&, Datetime&, Color&, Color&);
static std::string renderTotal           (const std::string&, const Rules&, time_t);
static std::string renderSubTotal        (const std::string&, const Rules&, int, int, time_t);
static void        renderExclusionBlocks (const std::string&, const Rules&, std::vector <Composite>&, Palette&, const Datetime&, int, int, const std::vector <Range>&);
static void        renderInterval        (const std::string&, const Rules&, std::vector <Composite>&, const Datetime&, const Interval&, std::map <std::string, Color>&, int, time_t&, bool);
       std::string renderHolidays        (const std::string&, const Rules&, const Interval&);
static std::string renderSummary         (const std::string&, const Rules&, const std::string&, const Interval&, const std::vector <Range>&, const std::vector <Interval>&, bool);

////////////////////////////////////////////////////////////////////////////////
int CmdChartDay (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose the current day.
  auto filter = getFilter (cli);
  if (! filter.range.is_started ())
  {
    if (rules.has ("reports.day.range"))
      expandIntervalHint (rules.get ("reports.day.range"), filter.range);
    else
      filter.range = Range (Datetime ("today"), Datetime ("tomorrow"));
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
  if (! filter.range.is_started ())
  {
    if (rules.has ("reports.week.range"))
      expandIntervalHint (rules.get ("reports.week.range"), filter.range);
    else
      filter.range = Range (Datetime ("sow"), Datetime ("eow"));
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
  if (! filter.range.is_started ())
  {
    if (rules.has ("reports.month.range"))
      expandIntervalHint (rules.get ("reports.month.range"), filter.range);
    else
      filter.range = Range (Datetime ("som"), Datetime ("eom"));
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
  auto exclusions = getAllExclusions (rules, filter.range);
  auto tracked    = getTracked (database, rules, filter);

  // Map tags to colors.
  auto palette = createPalette (rules);
  auto tag_colors = createTagColorMap (rules, palette, tracked);
  Color colorToday (palette.enabled ? rules.get ("theme.colors.today") : "");
  Color colorHoliday (palette.enabled ? rules.get ("theme.colors.holiday") : "");

  // Determine hours shown.
  int first_hour = 0;
  int last_hour  = 23;
  determineHourRange (type, rules, filter, tracked, first_hour, last_hour);

  // Render the axis.
  std::cout << '\n';
  if (rules.get ("reports." + type + ".axis") != "internal")
    renderAxis (type,
                rules,
                palette,
                std::string ((rules.getBoolean ("reports." + type + ".month")   ? 4 : 0) +
                             (rules.getBoolean ("reports." + type + ".week")    ? 4 : 0) +
                             (rules.getBoolean ("reports." + type + ".day")     ? 3 : 0) +
                             (rules.getBoolean ("reports." + type + ".weekday") ? 4 : 0),
                             ' '),
                first_hour,
                last_hour);

  // For rendering labels on edge detection.
  Datetime previous {0};

  // Is the :blank hint being used?
  bool blank = findHint (cli, ":blank");
  bool ids   = findHint (cli, ":ids");

  // Determine how much space is occupied by the left-margin labels.
  int indent = (rules.getBoolean ("reports." + type + ".month")   ? 4 : 0) +
               (rules.getBoolean ("reports." + type + ".week")    ? 4 : 0) +
               (rules.getBoolean ("reports." + type + ".day")     ? 3 : 0) +
               (rules.getBoolean ("reports." + type + ".weekday") ? 4 : 0);

  auto cell = rules.getInteger ("reports." + type + ".cell");
  if (cell < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

  auto chars_per_hour = 60 / cell;

  // Each day is rendered separately.
  time_t total_work = 0;
  for (Datetime day = filter.range.start; day < filter.range.end; day++)
  {
    // Render the exclusion blocks.
    int num_lines = 1;
    if (rules.has ("reports." + type + ".lines"))
      num_lines = rules.getInteger ("reports." + type + ".lines", num_lines);

    if (num_lines < 1)
      throw format ("Invalid value for 'reports.{1}.lines': '{2}'", type, rules.get ("reports." + type + ".lines"));

    int spacing = 1;
    if (rules.has ("reports." + type + ".spacing"))
      spacing = rules.getInteger ("reports." + type + ".spacing");

    // Add an empty string with no color, to reserve width, so this function
    // can simply concatenate to lines[i].str ().
    int total_width = (last_hour - first_hour + 1) * (chars_per_hour + spacing) - 1;
    std::vector <Composite> lines (num_lines);
    for (int i = 0; i < num_lines; ++i)
      lines[i].add (std::string (total_width, ' '), 0, Color ());

    renderExclusionBlocks (type, rules, lines, palette, day, first_hour, last_hour, exclusions);

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

    auto labelMonth = renderMonth (type, rules, previous, day);
    auto labelDay   = renderDayName (type, rules, day, colorToday, colorHoliday);

    std::cout << labelMonth
              << labelDay
              << lines[0].str ();

    if (lines.size () > 1)
      for (unsigned int i = 1; i < lines.size (); ++i)
        std::cout << "\n"
                  << std::string (indent, ' ')
                  << lines[i].str ();

    std::cout << renderTotal (type, rules, work)
              << '\n';

    previous = day;
    total_work += work;
  }

  std::cout << renderSubTotal (type, rules, first_hour, last_hour, total_work)
            << renderHolidays (type, rules, filter)
            << renderSummary (type, rules, std::string (indent, ' '), filter, exclusions, tracked, blank);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Scan all tracked intervals, looking for the earliest and latest hour into
// which an interval extends.
static void determineHourRange (
  const std::string& type,
  const Rules& rules,
  const Interval& filter,
  const std::vector <Interval>& tracked,
  int& first_hour,
  int& last_hour)
{
  if (rules.get ("reports." + type + ".hours") == "auto")
  {
    // Default to the full day.
    first_hour = 0;
    last_hour = 23;

    // If there is no data, show the whole day.
    if (! tracked.empty ())
    {
      // Get the extreme time range for the filtered data.
      first_hour = 23;
      last_hour  = 0;
      for (Datetime day = filter.range.start; day < filter.range.end; day++)
      {
        auto day_range = getFullDay (day);

        for (auto& track : tracked)
        {
          if (day_range.overlap (track.range))
          {
            Interval clipped = clip (track, day_range);
            if (track.range.is_open ())
              clipped.range.end = Datetime ();

            if (clipped.range.start.hour () < first_hour)
              first_hour = clipped.range.start.hour ();

            if (! clipped.range.is_open () &&
                clipped.range.end.hour () > last_hour)
              last_hour = clipped.range.end.hour ();
          }
        }
      }

      if (first_hour == 23 &&
          last_hour == 0)
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

    debug (format ("Day range is from {1}:00 - {2}:00", first_hour, last_hour));
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderAxis (
  const std::string& type,
  const Rules& rules,
  Palette& palette,
  const std::string& indent,
  int first_hour,
  int last_hour)
{
  auto cell = rules.getInteger ("reports." + type + ".cell");
  if (cell < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

  auto chars_per_hour = 60 / cell;

  auto spacing = rules.getInteger ("reports." + type + ".spacing");
  auto showTotal = rules.getBoolean ("reports." + type + ".totals");
  Color colorLabel (palette.enabled ? rules.get ("theme.colors.label") : "");
  Color colorToday (palette.enabled ? rules.get ("theme.colors.today") : "");

  auto current_hour = Datetime ().hour ();

  std::cout << indent;
  for (int hour = first_hour; hour <= last_hour; hour++)
    if (hour == current_hour)
      std::cout << colorToday.colorize (leftJustify (hour, chars_per_hour + spacing));
    else
      std::cout << colorLabel.colorize (leftJustify (hour, chars_per_hour + spacing));

  if (showTotal)
    std::cout << "  " << colorLabel.colorize ("Total");

  std::cout << '\n';
}

////////////////////////////////////////////////////////////////////////////////
// Includes trailing separator space.
static std::string renderMonth (
  const std::string& type,
  const Rules& rules,
  const Datetime& previous,
  const Datetime& day)
{
  auto showMonth = rules.getBoolean ("reports." + type + ".month");
  auto showWeek  = rules.getBoolean ("reports." + type + ".week");

  std::stringstream out;
  if (showMonth)
    out << (previous.month () != day.month () ? day.monthNameShort (day.month ()) : "   ")
        << ' ';

  if (showWeek)
    out << (previous.week () != day.week () ? leftJustify (format ("W{1}", day.week ()), 3) : "   ")
        << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Today should be highlighted.
// Includes trailing separator space.
static std::string renderDayName (
  const std::string& type,
  const Rules& rules,
  Datetime& day,
  Color& colorToday,
  Color& colorHoliday)
{
  auto showDay     = rules.getBoolean ("reports." + type + ".day");
  auto showWeekday = rules.getBoolean ("reports." + type + ".weekday");

  Color color;
  if (day.sameDay (Datetime ()))
    color = colorToday;
  else if (dayIsHoliday (rules, day))
    color = colorHoliday;

  std::stringstream out;
  if (showWeekday)
    out << color.colorize (day.dayNameShort (day.dayOfWeek ()))
        << ' ';

  if (showDay)
    out << color.colorize (rightJustify (day.day (), 2))
        << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static std::string renderTotal (
  const std::string& type,
  const Rules& rules,
  time_t work)
{
  auto showTotal = rules.getBoolean ("reports." + type + ".totals");

  std::stringstream out;
  if (showTotal && work)
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
  const std::string& type,
  const Rules& rules,
  int first_hour,
  int last_hour,
  time_t total_work)
{
  std::stringstream out;
  if (rules.getBoolean ("reports." + type + ".totals"))
  {
    int indent = (rules.getBoolean ("reports." + type + ".month")   ? 4 : 0) +
                 (rules.getBoolean ("reports." + type + ".week")    ? 4 : 0) +
                 (rules.getBoolean ("reports." + type + ".day")     ? 3 : 0) +
                 (rules.getBoolean ("reports." + type + ".weekday") ? 4 : 0);
    int spacing = rules.getInteger ("reports." + type + ".spacing");

    auto cell = rules.getInteger ("reports." + type + ".cell");
    if (cell < 1)
      throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

    auto chars_per_hour = 60 / cell;

    std::string pad (indent + ((last_hour - first_hour + 1) * (chars_per_hour + spacing)) + 1, ' ');

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
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static void renderExclusionBlocks (
  const std::string& type,
  const Rules& rules,
  std::vector <Composite>& lines,
  Palette& palette,
  const Datetime& day,
  int first_hour,
  int last_hour,
  const std::vector <Range>& excluded)
{
  auto cell = rules.getInteger ("reports." + type + ".cell");
  if (cell < 1)
    throw format ("The value for 'reports.{1}.cell' must be at least 1.", type);

  auto chars_per_hour = 60 / cell;

  auto spacing = rules.getInteger ("reports." + type + ".spacing");
  auto axis = rules.get ("reports." + type + ".axis");
  Color colorExc (palette.enabled ? rules.get ("theme.colors.exclusion") : "");
  Color colorLabel (palette.enabled ? rules.get ("theme.colors.label") : "");

  // Render the exclusion blocks.
  for (int hour = first_hour; hour <= last_hour; hour++)
  {
    // Construct a range representing a single 'hour', of 'day'.
    Range r (Datetime (day.year (), day.month (), day.day (), hour, 0, 0),
             Datetime (day.year (), day.month (), day.day (), hour + 1, 0, 0));

    if (axis == "internal")
    {
      auto label = format ("{1}", hour);
      int offset = (hour - first_hour) * (chars_per_hour + spacing);
      lines[0].add (label, offset, colorLabel);
    }

    for (auto& exc : excluded)
    {
      if (exc.overlap (r))
      {
        // Determine which of the character blocks included.
        auto sub_hour = exc.intersect (r);
        auto start_block = quantizeToNMinutes (sub_hour.start.minute (), cell) / cell;
        auto end_block   = quantizeToNMinutes (sub_hour.end.minute () == 0 ? 60 : sub_hour.end.minute (), cell) / cell;

        int offset = (hour - first_hour) * (chars_per_hour + spacing) + start_block;
        int width = end_block - start_block;
        std::string block (width, ' ');

        for (auto& line : lines)
          line.add (block, offset, colorExc);

        if (axis == "internal")
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
  if (! day_range.overlap (track.range) ||
      (track.range.is_open () && day > now))
    return;

  // If the track is open and day is today, then closed the track now, otherwise
  // it will be rendered until midnight.
  Interval clipped = clip (track, day_range);
  if (track.range.is_open ())
  {
    if (day_range.start.sameDay (now))
      clipped.range.end = now;
    else
      clipped.range.end = day_range.end;
  }

  auto start_mins = (clipped.range.start.hour () - first_hour) * 60 + clipped.range.start.minute ();
  auto end_mins   = (clipped.range.end.hour ()   - first_hour) * 60 + clipped.range.end.minute ();

  if (clipped.range.end.hour () == 0)
    end_mins += (clipped.range.end.day() + (clipped.range.end.month () - clipped.range.start.month () - 1) * clipped.range.start.day ()) * 24 * 60;

  work = clipped.range.total ();

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
      label = format ("@{1}", track.id);

    for (auto& tag : track.tags ())
    {
      if (! label.empty ())
        label += ' ';

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
          lines[i].add (leftJustify (text_lines[i], width), start_offset, colorTrack);
        else
          lines[i].add (std::string (width, ' '), start_offset, colorTrack);
      }

      // An open interval gets a "..." in the bottom right corner, or
      // whatever fits.
      if (track.range.is_open ())
        lines.back ().add ("+", start_offset + width - 1, colorTrack);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string renderHolidays (
  const std::string& type,
  const Rules& rules,
  const Interval& filter)
{
  std::stringstream out;
  if (rules.getBoolean ("reports." + type + ".holidays"))
  {
    for (auto& entry : rules.all ("holidays."))
    {
      auto first_dot = entry.find ('.');
      auto last_dot = entry.rfind ('.');
      if (last_dot != std::string::npos)
      {
        auto date = entry.substr (last_dot + 1);
        std::replace (date.begin (), date.end (), '_', '-');
        Datetime holiday (date);
        if (holiday >= filter.range.start &&
            holiday <= filter.range.end)
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
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
static std::string renderSummary (
  const std::string& type,
  const Rules& rules,
  const std::string& indent,
  const Interval& filter,
  const std::vector <Range>& exclusions,
  const std::vector <Interval>& tracked,
  bool blank)
{
  std::stringstream out;
  if (rules.getBoolean ("reports." + type + ".summary"))
  {
    time_t total_unavailable = 0;
    for (auto& exclusion : exclusions)
      if (filter.range.overlap (exclusion))
        total_unavailable += filter.range.intersect (exclusion).total ();

    time_t total_worked = 0;
    if (! blank)
    {
      for (auto& interval : tracked)
      {
        if (filter.range.overlap (interval.range))
        {
          Interval clipped = clip (interval, filter.range);
          if (interval.range.is_open ())
            clipped.range.end = Datetime ();

          total_worked += clipped.range.total ();
        }
      }
    }

    auto total_available = filter.range.total () - total_unavailable;
    assert (total_available >= 0);
    auto total_remaining = total_available - total_worked;

    out << '\n'
        << indent << "Tracked   "
        << std::setw (13) << std::setfill (' ') << Duration (total_worked).formatHours ()    << '\n';

    if (total_remaining >= 0)
      out << indent << "Available "
          << std::setw (13) << std::setfill (' ') << Duration (total_remaining).formatHours () << '\n';

    out << indent << "Total     "
        << std::setw (13) << std::setfill (' ') << Duration (total_available).formatHours ()   << '\n'
        << '\n';
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
