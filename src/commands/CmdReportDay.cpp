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
static void renderExclusionBlocks (const Rules&, Composite&, Composite&, Palette&, const Datetime&, int, int, const std::vector <Range>&);
static void renderInterval        (const Rules&, Composite&, Composite&, const Datetime&, const Interval&, int, Palette&, std::map <std::string, Color>&);
static void renderSummary         (const std::string&, const std::vector <Range>&, const std::vector <Interval>&);

////////////////////////////////////////////////////////////////////////////////
int CmdReportDay (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose 'today'.
  auto filter = getFilter (cli);
  if (! filter.range.started ())
    filter.range = Range (Datetime ("today"), Datetime ("tomorrow"));

  // Load the data.
  auto exclusions = getAllExclusions (rules, filter.range);
  auto tracked    = getTrackedIntervals (database, rules, filter);

  // Create a color palette.
  auto palette = createPalette (rules);
  Color colorExc   (palette.enabled ? rules.get ("theme.colors.exclusion") : "");
  Color colorLabel (palette.enabled ? rules.get ("theme.colors.label")     : "");

  // Map tags to colors.
  auto tag_colors = createTagColorMap (rules, palette, tracked);

  // Determine hours shown.
  int first_hour = 0;
  int last_hour  = 23;
  if (! rules.getBoolean ("report.day.24hours"))
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
//  std::cout << "# hours: " << first_hour << " - " << last_hour << "\n";

  // Render the axis.
  std::cout << '\n';
  std::string indent = "  ";
  if (rules.get ("report.day.style") != "compact")
    renderAxis (rules, palette, indent, first_hour, last_hour);

  // Each day is rendered separately.
  for (Datetime day = filter.range.start; day < filter.range.end; day++)
  {
//    std::cout << "# day " << day.toISOLocalExtended () << "\n";

    // Render the exclusion blocks.
    Composite line1;
    Composite line2;
    renderExclusionBlocks (rules, line1, line2, palette, day, first_hour, last_hour, exclusions);

    for (auto& track : tracked)
      renderInterval (rules, line1, line2, day, track, first_hour, palette, tag_colors);

    std::cout << indent << line1.str () << '\n'
              << indent << line2.str () << '\n'
              << '\n';
  }

  if (rules.getBoolean ("report.day.summary"))
    renderSummary (indent, exclusions, tracked);

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
  auto spacing = rules.getInteger ("report.day.spacing");
  Color colorLabel (palette.enabled ? rules.get ("theme.colors.label")     : "");

  std::cout << indent;
  for (int hour = first_hour; hour <= last_hour; hour++)
    std::cout << colorLabel.colorize (leftJustify (hour, 4 + spacing));

  std::cout << '\n';
}

////////////////////////////////////////////////////////////////////////////////
static void renderExclusionBlocks (
  const Rules& rules,
  Composite& line1,
  Composite& line2,
  Palette& palette,
  const Datetime& day,
  int first_hour,
  int last_hour,
  const std::vector <Range>& excluded)
{
  auto spacing = rules.getInteger ("report.day.spacing");
  auto style = rules.get ("report.day.style");
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

        line1.add (block, offset, colorExc);
        line2.add (block, offset, colorExc);

        if (style == "compact")
        {
          auto label = format ("{1}", hour);
          if (start_block == 0 &&
              width >= static_cast <int> (label.length ()))
            line1.add (label, offset, colorExc);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderInterval (
  const Rules& rules,
  Composite& line1,
  Composite& line2,
  const Datetime& day,
  const Interval& track,
  int first_hour,
  Palette& palette,
  std::map <std::string, Color>& tag_colors)
{
  auto spacing = rules.getInteger ("report.day.spacing");

  // Make sure the track only represents one day.
  Datetime eod {day};
  eod++;
  Range day_range (day, eod);
  Interval clipped = clip (track, day_range);

  // TODO track may have started days ago.
  auto start_hour = clipped.range.start.hour ();
  auto start_min  = clipped.range.start.minute ();
  auto end_hour   = clipped.range.end.hour ();
  auto end_min    = clipped.range.end.minute ();

  auto start_block = quantizeTo15Minutes (start_min) / 15;
  auto end_block   = quantizeTo15Minutes (end_min == 0 ? 60 : end_min) / 15;

  int start_offset = (start_hour - first_hour) * (4 + spacing) + start_block;
  int end_offset   = (end_hour - 1 - first_hour) * (4 + spacing) + end_block;

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

      line1.add (label1, start_offset, colorTrack);
      line2.add (label2, start_offset, colorTrack);

      // An open interval gets a "..." in the bottom right corner, or
      // whatever fits.
      if (! track.range.ended ())
        line2.add ("+", width - 1, colorTrack);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderSummary (
  const std::string& indent,
  const std::vector <Range>& exclusions,
  const std::vector <Interval>& tracked)
{
  time_t unavailable = 0;
  for (auto& exclusion : exclusions)
    unavailable += exclusion.total ();

  time_t worked = 0;
  for (auto& interval : tracked)
    worked += interval.range.total ();

  auto all_day = 86400 - unavailable;
  auto remaining = all_day - worked;

  std::cout << indent << "Tracked   "
            << std::setw (13) << std::setfill (' ') << Duration (worked).format ()    << '\n'
            << indent << "Remaining "
            << std::setw (13) << std::setfill (' ') << Duration (remaining).format () << '\n'
            << indent << "Total     "
            << std::setw (13) << std::setfill (' ') << Duration (all_day).format ()   << '\n'
            << '\n';
}

////////////////////////////////////////////////////////////////////////////////
