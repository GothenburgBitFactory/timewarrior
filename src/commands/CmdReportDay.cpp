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
#include <Range.h>
#include <commands.h>
#include <timew.h>
#include <shared.h>
#include <format.h>
#include <algorithm>
#include <iostream>

static void renderAxis (const std::string&, int, int, Color&);
static void renderExclusionBlocks (Composite&, Composite&, const Datetime&, int, int, const std::vector <Range>&, Color&);
static void renderInterval (Composite&, Composite&, const Interval&, int, Palette&, std::map <std::string, Color>&);
static void renderSummary (const std::string&);

////////////////////////////////////////////////////////////////////////////////
int CmdReportDay (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, and if empty, choose 'today'.
  auto filter = createFilterIntervalFromCLI (cli);
  if (! filter.range.started ())
    filter.range = Range (Datetime ("today"), Datetime ("tomorrow"));

  // Load the data.
  auto timeline = createTimelineFromData (rules, database, filter);
  auto tracked  = timeline.tracked (rules);
  auto excluded = timeline.excluded (rules);

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

  // Render the axis.
  std::cout << '\n';
  std::string indent = "  ";
  renderAxis (indent, first_hour, last_hour, colorLabel);

  // Each day is rendered separately.
  for (Datetime day = filter.range.start; day < filter.range.end; day++)
  {
    // Render the exclusion blocks.
    Composite line1;
    Composite line2;
    renderExclusionBlocks (line1, line2, day, first_hour, last_hour, excluded, colorExc);

    for (auto& track : tracked)
      renderInterval (line1, line2, track, first_hour, palette, tag_colors);

    std::cout << indent << line1.str () << '\n'
              << indent << line2.str () << '\n'
              << '\n';
  }

  renderSummary (indent);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
static void renderAxis (
  const std::string& indent,
  int first_hour,
  int last_hour,
  Color& colorLabel)
{
  std::cout << indent;
  for (int hour = first_hour; hour <= last_hour; hour++)
    std::cout << colorLabel.colorize (leftJustify (hour, 5));

  std::cout << '\n';
}

////////////////////////////////////////////////////////////////////////////////
static void renderExclusionBlocks (
  Composite& line1,
  Composite& line2,
  const Datetime& day,
  int first_hour,
  int last_hour,
  const std::vector <Range>& excluded,
  Color& colorExc)
{
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

        int offset = (hour - first_hour) * 5 + start_block;
        std::string block (end_block - start_block, ' ');

        line1.add (block, offset, colorExc);
        line2.add (block, offset, colorExc);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderInterval (
  Composite& line1,
  Composite& line2,
  const Interval& track,
  int first_hour,
  Palette& palette,
  std::map <std::string, Color>& tag_colors)
{
  // TODO track may have started days ago.
  auto start_hour = track.range.start.hour ();
  auto start_min  = track.range.start.minute ();
  auto end_hour   = track.range.end.hour ();
  auto end_min    = track.range.end.minute ();

  auto start_block = quantizeTo15Minutes (start_min) / 15;
  auto end_block   = quantizeTo15Minutes (end_min == 0 ? 60 : end_min) / 15;

  int start_offset = (start_hour - first_hour) * 5 + start_block;
  int end_offset   = (end_hour - 1 - first_hour) * 5 + end_block;

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
      {
/*
        line2.add ("+", width - 1, colorTrack);
*/
        line2.add ("→", width - 1, colorTrack);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void renderSummary (const std::string& indent)
{
  // TODO Summary, missing.
  std::cout << indent << "Tracked\n"
            << indent << "Untracked\n"
            << indent << "Total\n"
            << '\n';
}

////////////////////////////////////////////////////////////////////////////////
