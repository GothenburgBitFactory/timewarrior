////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <Chart.h>
#include <Composite.h>
#include <Duration.h>
#include <cassert>
#include <format.h>
#include <iomanip>
#include <shared.h>
#include <timew.h>
#include <utf8.h>

////////////////////////////////////////////////////////////////////////////////
Chart::Chart (const ChartConfig& configuration) :
  reference_datetime (configuration.reference_datetime),
  with_label_month (configuration.with_label_month),
  with_label_week (configuration.with_label_week),
  with_label_weekday (configuration.with_label_weekday),
  with_label_day (configuration.with_label_day),
  with_ids (configuration.with_ids),
  with_summary (configuration.with_summary),
  with_holidays (configuration.with_holidays),
  with_totals (configuration.with_totals),
  with_internal_axis (configuration.with_internal_axis),
  show_intervals (configuration.show_intervals),
  determine_hour_range (configuration.determine_hour_range),
  minutes_per_char (configuration.minutes_per_char),
  spacing (configuration.spacing),
  num_lines (configuration.num_lines),
  color_today (configuration.color_today),
  color_holiday (configuration.color_holiday),
  color_label (configuration.color_label),
  color_exclusion (configuration.color_exclusion),
  tag_colors (configuration.tag_colors),
  cell_width (60 / minutes_per_char + spacing),
  reference_hour (reference_datetime.hour ())
{ }

std::string Chart::render (
  const Range& range,
  const std::vector <Interval>& tracked,
  const std::vector <Range>& exclusions,
  const std::map <Datetime, std::string>& holidays)
{
  // Determine hours shown.
  auto hour_range = determine_hour_range
                    ? determineHourRange (range, tracked)
                    : std::make_pair (0, 23);

  int first_hour = hour_range.first;
  int last_hour = hour_range.second;

  debug (format ("Day range is from {1}:00 - {2}:00", first_hour, last_hour));

  const auto indent_size = getIndentSize ();
  const auto total_width = (last_hour - first_hour + 1) * (cell_width);
  const auto padding_size = indent_size + total_width + 1;
  const auto indent = std::string (indent_size, ' ');

  std::stringstream out;

  out << '\n';

  // Render the axis.
  if (! with_internal_axis)
  {
    out << indent << renderAxis (first_hour, last_hour);
  }

  // For rendering labels on edge detection.
  Datetime previous {0};

  // Each day is rendered separately.
  time_t total_work = 0;

  for (Datetime day = range.start; day < range.end; day++)
  {
    // Render the exclusion blocks.

    // Add an empty string with no color, to reserve width, so this function
    // can simply concatenate to lines[i].str ().
    std::vector <Composite> lines (num_lines);
    for (int i = 0; i < num_lines; ++i)
    {
      lines[i].add (std::string (total_width, ' '), 0, Color ());
    }

    renderExclusionBlocks (lines, day, first_hour, last_hour, exclusions);

    time_t work = 0;
    if (! show_intervals)
    {
      for (auto& track : tracked)
      {
        time_t interval_work = 0;
        renderInterval (lines, day, track, first_hour, interval_work);
        work += interval_work;
      }
    }

    auto color_day = getDayColor (day, holidays);

    auto labelMonth = with_label_month ? renderMonth (previous, day) : "";
    auto labelWeek = with_label_week ? renderWeek (previous, day) : "";
    auto labelWeekday = with_label_weekday ? renderWeekday (day, color_day) : "";
    auto labelDay = with_label_day ? renderDay (day, color_day) : "";

    out << labelMonth
        << labelWeek
        << labelWeekday
        << labelDay
        << lines[0].str ();

    if (lines.size () > 1)
    {
      for (unsigned int i = 1; i < lines.size (); ++i)
      {
        out << "\n"
            << indent
            << lines[i].str ();
      }
    }

    out << (with_totals ? renderTotal (work) : "")
        << '\n';

    previous = day;
    total_work += work;
  }

  out << (with_totals ? renderSubTotal (total_work, std::string (padding_size, ' ')) : "")
      << (with_holidays ? renderHolidays (holidays) : "")
      << (with_summary ? renderSummary (indent, range, exclusions, tracked) : "");

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
unsigned long Chart::getIndentSize ()
{
  return (with_label_month ? 4 : 0) +
         (with_label_week ? 4 : 0) +
         (with_label_day ? 3 : 0) +
         (with_label_weekday ? 4 : 0);
}

////////////////////////////////////////////////////////////////////////////////
// Scan all tracked intervals, looking for the earliest and latest hour into
// which an interval extends.
std::pair <int, int> Chart::determineHourRange (
  const Range& range,
  const std::vector <Interval>& tracked)
{
  // If there is no data, show the whole day.
  if (tracked.empty ())
  {
    return std::make_pair (0, 23);
  }

  // Get the extreme time range for the filtered data.
  auto first_hour = 23;
  auto last_hour = 0;

  for (Datetime day = range.start; day < range.end; day++)
  {
    auto day_range = getFullDay (day);

    for (auto& track : tracked)
    {
      Interval test {track};

      if (test.is_open ())
      {
        test.end = reference_datetime;
      }

      if (day_range.overlaps (test))
      {
        Interval clipped = clip (test, day_range);

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
    first_hour = reference_hour;
    last_hour = std::min (first_hour + 1, 23);
  }
  else
  {
    first_hour = std::max (first_hour - 1, 0);
    last_hour = std::min (last_hour, 23);
  }

  return std::make_pair (first_hour, last_hour);
}

////////////////////////////////////////////////////////////////////////////////
std::string Chart::renderAxis (const int first_hour, const int last_hour)
{
  std::stringstream out;

  for (int hour = first_hour; hour <= last_hour; hour++)
  {
    auto color = getHourColor (hour);
    out << color.colorize (leftJustify (hour, cell_width));
  }

  if (with_totals)
  {
    out << "  " << color_label.colorize ("Total");
  }

  out << '\n';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Chart::renderMonth (const Datetime& previous, const Datetime& day)
{
  const auto show_month = previous.month () != day.month ();

  std::stringstream out;

  out << (show_month ? Datetime::monthNameShort (day.month ()) : "   ")
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Chart::renderWeek (const Datetime& previous, const Datetime& day)
{
  const auto show_week = previous.week () != day.week ();

  std::stringstream out;

  out << (show_week ? leftJustify (format ("W{1}", day.week ()), 3) : "   ")
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Chart::renderWeekday (Datetime& day, const Color& color)
{
  std::stringstream out;

  out << color.colorize (Datetime::dayNameShort (day.dayOfWeek ()))
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Chart::renderDay (Datetime& day, const Color& color)
{
  std::stringstream out;

  out << color.colorize (rightJustify (day.day (), 2))
      << ' ';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
Color Chart::getDayColor (
  const Datetime& day,
  const std::map <Datetime, std::string>& holidays)
{
  if (day.sameDay (reference_datetime))
  {
    return color_today;
  }

  for (auto& entry : holidays)
  {
    if (day.sameDay (entry.first))
    {
      return color_holiday;
    }
  }

  return Color {};
}

////////////////////////////////////////////////////////////////////////////////
Color Chart::getHourColor (int hour) const
{
  return hour == reference_hour ? color_today : color_label;
}

////////////////////////////////////////////////////////////////////////////////
std::string Chart::renderTotal (time_t work)
{
  std::stringstream out;

  if (work)
  {
    int hours = work / 3600;
    int minutes = (work % 3600) / 60;
    out << " "
        << std::setw (3) << std::setfill (' ') << hours
        << ':'
        << std::setw (2) << std::setfill ('0') << minutes;
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Chart::renderSubTotal (
  time_t total_work,
  const std::string& padding)
{
  std::stringstream out;

  int hours = total_work / 3600;
  int minutes = (total_work % 3600) / 60;

  out << padding
      << Color ("underline").colorize ("      ")
      << '\n'
      << padding
      << std::setw (3) << std::setfill (' ') << hours
      << ':'
      << std::setw (2) << std::setfill ('0') << minutes
      << '\n';

  return out.str ();
}

void Chart::renderExclusionBlocks (
  std::vector <Composite>& lines,
  const Datetime& day,
  int first_hour,
  int last_hour,
  const std::vector <Range>& exclusions)
{
  // Render the exclusion blocks.
  for (int hour = first_hour; hour <= last_hour; hour++)
  {
    // Construct a range representing a single 'hour', of 'day'.
    Range hour_range (Datetime (day.year (), day.month (), day.day (), hour, 0, 0),
                      Datetime (day.year (), day.month (), day.day (), hour + 1, 0, 0));

    if (with_internal_axis)
    {
      auto label = format ("{1}", hour);
      int offset = (hour - first_hour) * cell_width;
      lines[0].add (label, offset, color_label);
    }

    for (auto& exclusion : exclusions)
    {
      if (exclusion.overlaps (hour_range))
      {
        // Determine which of the character blocks included.
        auto sub_hour = exclusion.intersect (hour_range);
        auto start_block = quantizeToNMinutes (sub_hour.start.minute (), minutes_per_char) / minutes_per_char;
        auto end_block = quantizeToNMinutes (sub_hour.end.minute () == 0 ? 60 : sub_hour.end.minute (), minutes_per_char) / minutes_per_char;

        int offset = (hour - first_hour) * cell_width + start_block;
        int width = end_block - start_block;
        std::string block (width, ' ');

        for (auto& line : lines)
        {
          line.add (block, offset, color_exclusion);
        }

        if (with_internal_axis)
        {
          auto label = format ("{1}", hour);
          if (start_block == 0 &&
              width >= static_cast <int> (label.length ()))
          {
            lines[0].add (label, offset, color_exclusion);
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void Chart::renderInterval (
  std::vector <Composite>& lines,
  const Datetime& day,
  const Interval& track,
  const int first_hour,
  time_t& work)
{
  // Ignore any track that doesn't overlap with day.
  auto day_range = getFullDay (day);
  if (! day_range.overlaps (track) || (track.is_open () && day > reference_datetime))
  {
    return;
  }

  // If the track is open and day is today, then closed the track now, otherwise
  // it will be rendered until midnight.
  Interval clipped = clip (track, day_range);
  if (track.is_open ())
  {
    if (day_range.start.sameDay (reference_datetime))
    {
      clipped.end = reference_datetime;
    }
    else
    {
      clipped.end = day_range.end;
    }
  }

  auto start_mins = (clipped.start.hour () - first_hour) * 60 + clipped.start.minute ();
  auto end_mins = (clipped.end.hour () - first_hour) * 60 + clipped.end.minute ();

  if (clipped.end.hour () == 0)
  {
    end_mins += (clipped.end.day () + (clipped.end.month () - clipped.start.month () - 1) * clipped.start.day ()) * 24 * 60;
  }

  work = clipped.total ();

  auto start_block = quantizeToNMinutes (start_mins, minutes_per_char) / minutes_per_char;
  auto end_block = quantizeToNMinutes (end_mins == start_mins ? start_mins + 60 : end_mins, minutes_per_char) / minutes_per_char;

  int start_offset = start_block + (spacing * (start_mins / 60));
  int end_offset = end_block + (spacing * (end_mins / 60));

  if (end_offset > start_offset)
  {
    // Determine color of interval.
    Color colorTrack = chartIntervalColor (track.tags (), tag_colors);

    // Properly format the tags within the space.
    std::string label;
    if (with_ids)
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

      // --
      // The hang/memory consumption in #309 is due to a bug in libshared's wrapText
      // It would be best to make wrapText/extractText width be the count of characters on the screen (and not a byte width).
      // This fix will only show the tag if the utf8 character width is within the width (and it won't try to wrap),
      // but otherwise functions normally for text where the utf-8 width matches the byte length of the label.
      //
      size_t utf8_characters = utf8_text_width (label);
      if (static_cast <size_t> (width) >= utf8_characters)
      {
        text_lines.push_back (label);
      }
      else if (utf8_characters == label.size ())
      {
        wrapText (text_lines, label, width, false);
      }
      // --

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
std::string Chart::renderHolidays (const std::map <Datetime, std::string>& holidays)
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
std::string Chart::renderSummary (
  const std::string& indent,
  const Range& range,
  const std::vector <Range>& exclusions,
  const std::vector <Interval>& tracked)
{
  std::stringstream out;
  time_t total_unavailable = 0;

  for (auto& exclusion : exclusions)
  {
    if (range.overlaps (exclusion))
    {
      total_unavailable += range.intersect (exclusion).total ();
    }
  }

  time_t total_worked = 0;

  if (! show_intervals)
  {
    for (auto& interval : tracked)
    {
      if (range.overlaps (interval))
      {
        Interval clipped = clip (interval, range);
        if (interval.is_open ())
        {
          clipped.end = reference_datetime;
        }

        total_worked += clipped.total ();
      }
    }
  }

  auto total_available = range.total () - total_unavailable;
  assert (total_available >= 0);
  auto total_remaining = total_available - total_worked;

  out << '\n'
      << indent << "Tracked   "
      << std::setw (13) << std::setfill (' ') << Duration (total_worked).formatHours () << '\n';

  if (total_remaining >= 0)
  {
    out << indent << "Available "
        << std::setw (13) << std::setfill (' ') << Duration (total_remaining).formatHours () << '\n';
  }

  out << indent << "Total     "
      << std::setw (13) << std::setfill (' ') << Duration (total_available).formatHours () << '\n'
      << '\n';

  return out.str ();
}
