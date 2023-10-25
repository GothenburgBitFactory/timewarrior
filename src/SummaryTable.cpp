////////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2023, Gothenburg Bit Factory.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
//  https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <Datetime.h>
#include <SummaryTable.h>
#include <Table.h>
#include <format.h>
#include <timew.h>
#include <utf8.h>
#include <utility>

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder SummaryTable::builder ()
{
  return {};
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withWeekFormat (const std::string& format)
{
  _week_fmt = format;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withDateFormat (const std::string& format)
{
  _date_fmt = format;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withTimeFormat (const std::string& format)
{
  _time_fmt = format;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withAnnotations (const bool show)
{
  _show_annotations = show;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withIds (bool show, Color color)
{
  _show_ids = show;
  _color_id = color;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder & SummaryTable::Builder::withTags (bool show, std::map <std::string, Color>& colors)
{
  _show_tags = show;
  _color_tags = std::move (colors);
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withWeekdays (const bool show)
{
  _show_weekdays = show;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withWeeks (const bool show)
{
  _show_weeks = show;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder& SummaryTable::Builder::withRange (const Range& range)
{
  _range = range;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
SummaryTable::Builder & SummaryTable::Builder::withIntervals (const std::vector<Interval>& tracked)
{
  _tracked = tracked;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
Table SummaryTable::Builder::build ()
{
  const auto dates_col_offset = _show_weeks ? 1 : 0;
  const auto weekdays_col_offset = dates_col_offset;
  const auto ids_col_offset = weekdays_col_offset + (_show_weekdays ? 1: 0);
  const auto tags_col_offset = ids_col_offset + (_show_ids ? 1 : 0);
  const auto annotation_col_offset = tags_col_offset + (_show_tags ? 1 : 0);
  const auto start_col_offset = annotation_col_offset + (_show_annotations ? 1 : 0);

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

  int terminalWidth = getTerminalWidth ();

  Table table;
  table.width (terminalWidth);
  table.colorHeader (Color ("underline"));

  if (_show_weeks)
  {
    table.add ("Wk");
  }

  table.add ("Date");

  if (_show_weekdays)
  {
    table.add ("Day");
  }

  if (_show_ids)
  {
    table.add ("ID");
  }

  if (_show_tags)
  {
    table.add ("Tags");
  }

  if (_show_annotations)
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

  auto days_start = _range.is_started () ? _range.start : _tracked.front ().start;
  auto days_end   = _range.is_ended ()   ? _range.end   : _tracked.back ().end;

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
    for (auto& track : subset (day_range, _tracked))
    {
      // Make sure the track only represents one day.
      if ((track.is_open () && day > now))
      {
        continue;
      }

      row = table.addRow ();

      if (day != previous)
      {
        if (_show_weeks)
        {
          table.set (row, weeks_col_index, format (_week_fmt, day.week ()));
        }

        table.set (row, dates_col_index, day.toString (_date_fmt));

        if (_show_weekdays)
        {
          table.set (row, weekdays_col_index, Datetime::dayNameShort (day.dayOfWeek ()));
        }

        previous = day;
      }

      // Intersect track with day.
      auto today = day_range.intersect (track);

      if (track.is_open () && track.start > now)
      {
        today.end = track.start;
      }
      else if (track.is_open () && day <= now && today.end > now)
      {
        today.end = now;
      }

      if (_show_ids)
      {
        table.set (row, ids_col_index, format ("@{1}", track.id), _color_id);
      }

      if (_show_tags)
      {
        auto tags_string = join (", ", track.tags ());
        table.set (row, tags_col_index, tags_string, summaryIntervalColor (_color_tags, track.tags ()));
      }

      if (_show_annotations)
      {
        auto annotation = track.getAnnotation ();

        if (utf8_length (annotation) > 15)
        {
          annotation = utf8_substr (annotation, 0, 12) + "...";
        }

        table.set (row, annotation_col_index, annotation);
      }

      const auto total = today.total ();

      table.set (row, start_col_index, today.start.toString (_time_fmt));
      table.set (row, end_col_index, (track.is_open () ? "-" : today.end.toString (_time_fmt)));
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

  return table;
}

////////////////////////////////////////////////////////////////////////////////
