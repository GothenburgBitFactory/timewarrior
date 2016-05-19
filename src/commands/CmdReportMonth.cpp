////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, Paul Beckingham, Federico Hernandez.
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
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <Composite.h>
#include <Color.h>
#include <Duration.h>
#include <Range.h>
#include <commands.h>
#include <timew.h>
#include <shared.h>
#include <format.h>
#include <algorithm>
#include <iostream>
#include <iomanip>

static void renderAxis            (const Rules&, Palette&, const std::string&, int, int);
static void renderExclusionBlocks (const Rules&, Composite&, Palette&, const Datetime&, int, int, const std::vector <Range>&);
static void renderInterval        (const Rules&, Composite&, const Datetime&, const Interval&, Palette&, std::map <std::string, Color>&, time_t&);
static void renderSummary         (const std::string&, const Interval&, const std::vector <Range>&, const std::vector <Interval>&);

////////////////////////////////////////////////////////////////////////////////
int CmdReportMonth (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose 'today'.
  auto filter = getFilter (cli);
  if (! filter.range.is_started ())
    filter.range = Range (Datetime ("socm"), Datetime ("eocm"));

  // Load the data.
  auto exclusions = getAllExclusions (rules, filter.range);
  auto tracked    = getTracked (database, rules, filter);

  // Map tags to colors.
  auto palette = createPalette (rules);
  auto tag_colors = createTagColorMap (rules, palette, tracked);

  // Determine hours shown.
  int first_hour = 0;
  int last_hour  = 23;
  if (! rules.getBoolean ("reports.month.24hours"))
  {
    // Get the extreme time range for the filtered data.
    first_hour = 23;
    last_hour  = 0;
    for (auto& track : tracked)
    {
      if (track.range.start.hour () < first_hour)
        first_hour = track.range.start.hour ();

      if (track.range.end.hour () > last_hour)
        last_hour = track.range.end.hour ();
    }

    first_hour = std::max (first_hour - 1, 0);
    last_hour  = std::min (last_hour + 1, 23);
  }

  // Render the axis.
  std::cout << '\n';
  renderAxis (rules, palette, "               ", first_hour, last_hour);

  // For breaks.
  Datetime previous (filter.range.start);
  --previous;

  // Each day is rendered separately.
  time_t total_work = 0;
  for (Datetime day = filter.range.start; day < filter.range.end; day++)
  {
    // Render the exclusion blocks.
    Composite line;
    renderExclusionBlocks (rules, line, palette, day, first_hour, last_hour, exclusions);

    time_t work = 0;
    for (auto& track : tracked)
    {
      time_t interval_work = 0;
      renderInterval (rules, line, day, track, palette, tag_colors, interval_work);
      work += interval_work;
    }

    int hours = work / 3600;
    int minutes = (work % 3600) / 60;

    std::cout << (previous.month () != day.month () ? day.monthNameShort (day.month ()) : "   ")
              << ' '
              << (previous.week () != day.week () ? leftJustify (format ("W{1}", day.week ()), 3) : "   ")
              << ' '
              << day.dayNameShort (day.dayOfWeek ())
              << ' '
              << rightJustify (day.day (), 2)
              << ' '
              << line.str ()
              << "  ";

    if (work)
      std::cout << std::setw (3) << std::setfill (' ') << hours
                << ':'
                << std::setw (2) << std::setfill ('0') << minutes;

    std::cout << '\n';
    previous = day;
    total_work += work;
  }

  std::string pad (15 + ((last_hour - first_hour + 1) * (4 + rules.getInteger ("reports.month.spacing"))) + 1, ' ');
  std::cout << pad << "[4m      [0m\n";

  std::cout << pad
            << std::setw (3) << std::setfill (' ') << 123
            << ':'
            << std::setw (2) << std::setfill ('0') << 45
            << '\n';

  if (rules.getBoolean ("reports.month.summary"))
    renderSummary ("    ", filter, exclusions, tracked);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
static void renderAxis (
  const Rules& rules,
  Palette& palette,
  const std::string& indent,
  int first_hour,
  int last_hour)
{
  auto spacing = rules.getInteger ("reports.month.spacing");
  Color colorLabel (palette.enabled ? rules.get ("theme.colors.label")     : "");

  std::cout << indent;
  for (int hour = first_hour; hour <= last_hour; hour++)
    std::cout << colorLabel.colorize (leftJustify (hour, 4 + spacing));

  std::cout << "  " << colorLabel.colorize ("Total") << "\n";
}

////////////////////////////////////////////////////////////////////////////////
static void renderExclusionBlocks (
  const Rules& rules,
  Composite& line,
  Palette& palette,
  const Datetime& day,
  int first_hour,
  int last_hour,
  const std::vector <Range>& excluded)
{
  auto spacing = rules.getInteger ("reports.month.spacing");
  auto style = rules.get ("reports.month.style");
  Color colorExc (palette.enabled ? rules.get ("theme.colors.exclusion") : "");

  // Render the exclusion blocks.
  for (int hour = first_hour; hour <= last_hour; hour++)
  {
    // Construct a range representing a single 'hour', of 'day'.
    Range r (Datetime (day.year (), day.month (), day.day (), hour, 0, 0),
             Datetime (day.year (), day.month (), day.day (), hour + 1, 0, 0));

    for (auto& exc : excluded)
    {
      if (exc.overlap (r))
      {
        // Determine which of the four 15-min quarters are included.
        auto sub_hour = exc.intersect (r);
        auto start_block = quantizeTo15Minutes (sub_hour.start.minute ()) / 15;
        auto end_block   = quantizeTo15Minutes (sub_hour.end.minute () == 0 ? 60 : sub_hour.end.minute ()) / 15;

        int offset = (hour - first_hour) * (4 + spacing) + start_block;
        int width = end_block - start_block;
        std::string block (width, ' ');

        line.add (block, offset, colorExc);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderInterval (
  const Rules& rules,
  Composite& line,
  const Datetime& day,
  const Interval& track,
  Palette& palette,
  std::map <std::string, Color>& tag_colors,
  time_t& work)
{
  auto spacing = rules.getInteger ("reports.month.spacing");

  // Make sure the track only represents one day.
  auto day_range = getFullDay (day);
  if (! day_range.overlap (track.range))
    return;

  Interval clipped = clip (track, day_range);
  if (track.range.is_open ())
    clipped.range.end = Datetime ();

  auto start_mins = clipped.range.start.hour () * 60 + clipped.range.start.minute ();
  auto end_mins   = clipped.range.end.hour () * 60 + clipped.range.end.minute ();
  if (end_mins == 0)
    end_mins = (23 * 60) + 59;

  work = clipped.range.total ();

  int start_offset = (start_mins / 15) + (spacing * (start_mins / 60));
  int end_offset   = (end_mins   / 15) + (spacing * (end_mins   / 60));

  if (end_offset > start_offset)
  {
    // Determine color of interval.
    Color colorTrack;
    if (track.tags ().size ())
      // TODO Instead of using the first tag, look at them all, and choose one
      //      that has a color defined over any other.
      colorTrack = tag_colors [*(track.tags ().begin ())];
    else
      colorTrack = palette.next ();

    // Properly format the tags within the space.
    std::string label;
    for (auto& tag : track.tags ())
    {
      if (label != "")
        label += ' ';

      label += tag;
    }

    auto width = end_offset - start_offset;
    if (width)
    {
      std::vector <std::string> lines;
      wrapText (lines, label, width, false);

      std::string label1 (width, ' ');
      std::string label2 = label1;
      if (lines.size () > 0)
      {
        label1 = leftJustify (lines[0], width);

        if (lines.size () > 1)
          label2 = leftJustify (lines[1], width);
      }

      line.add (label1, start_offset, colorTrack);

      // An open interval gets a "..." in the bottom right corner, or
      // whatever fits.
      if (track.range.is_open ())
        line.add ("+", start_offset + width - 1, colorTrack);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderSummary (
  const std::string& indent,
  const Interval& filter,
  const std::vector <Range>& exclusions,
  const std::vector <Interval>& tracked)
{
  time_t total_unavailable = 0;
  for (auto& exclusion : exclusions)
    if (filter.range.overlap (exclusion))
      total_unavailable += filter.range.intersect (exclusion).total ();

  time_t total_worked = 0;
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

  auto total_available = filter.range.total () - total_unavailable;
  auto total_remaining = total_available - total_worked;

  std::cout << indent << "Tracked   "
            << std::setw (13) << std::setfill (' ') << Duration (total_worked).formatHours ()    << '\n';

  if (total_remaining >= 0)
    std::cout << indent << "Remaining "
              << std::setw (13) << std::setfill (' ') << Duration (total_remaining).formatHours () << '\n';

  std::cout << indent << "Total     "
            << std::setw (13) << std::setfill (' ') << Duration (total_available).formatHours ()   << '\n'
            << '\n';
}

////////////////////////////////////////////////////////////////////////////////
