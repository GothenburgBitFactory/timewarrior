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
#include <timew.h>
#include <shared.h>
#include <Datetime.h>
#include <Duration.h>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Consult rules to find any defined color for the given tah, and colorize it.
Color tagColor (const Rules& rules, const std::string& tag)
{
  Color c;
  std::string name = std::string ("tag.") + tag + ".color";
  if (rules.has (name))
    c = Color (rules.get (name));

  return c;
}

////////////////////////////////////////////////////////////////////////////////
// Summarize either an active or closed interval, for user feedback.
std::string intervalSummarize (const Rules& rules, const Interval& interval)
{
  std::stringstream out;
  if (interval.range.started ())
  {
    // Combine and colorize tags.
    std::string tags;
    for (auto& tag : interval.tags ())
    {
      if (tags != "")
        tags += " ";

      tags += tagColor (rules, tag).colorize (quoteIfNeeded (tag));
    }

    // Interval closed.
    if (interval.range.ended ())
    {
      Duration dur (Datetime (interval.range.end) - Datetime (interval.range.start));
      out << "Recorded " << tags << '\n'
          << "  Started " << interval.range.start.toISOLocalExtended () << '\n'
          << "  Ended   " << interval.range.end.toISOLocalExtended () << '\n'
          << "  Elapsed " << std::setw (19) << std::setfill (' ') << dur.format () << '\n';
    }

    // Interval open.
    else
    {
      Duration dur (Datetime () - interval.range.start);
      out << "Tracking " << tags << '\n'
          << "  Started " << interval.range.start.toISOLocalExtended () << '\n';

      if (dur.toTime_t () > 10)
        out << "  Current " << Datetime ().toISOLocalExtended () << '\n'
            << "  Elapsed " << std::setw (19) << std::setfill (' ') << dur.format () << '\n';
    }
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Convert a set of hints to equivalent date ranges.
bool expandIntervalHint (
  const std::string& hint,
  std::string& start,
  std::string& end)
{
  static std::map <std::string, std::vector <std::string>> hints
  {
    {":yesterday", {"yesterday", "today"}},
    {":day",       {"today",     "tomorrow"}},
    {":week",      {"socw",      "eocw"}},
    {":month",     {"socm",      "eocm"}},
    {":quarter",   {"socq",      "eocq"}},
    {":year",      {"socy",      "eocy"}},
  };

  if (hints.find (hint) != hints.end ())
  {
    start = hints[hint][0];
    end   = hints[hint][1];
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// A filter is just another interval, containing start, end and tags.
//
// Supported interval forms:
//   ["from"] <date> ["to"|"-" <date>]
//   ["from"] <date> "for" <duration>
//   <duration> ["before"|"after" <date>]
//
Interval createFilterIntervalFromCLI (const CLI& cli)
{
  Interval filter;
  std::string start;
  std::string end;
  std::string duration;

  std::vector <std::string> args;
  for (auto& arg : cli._args)
  {
    if (arg.hasTag ("BINARY") ||
        arg.hasTag ("CMD")    ||
        arg.hasTag ("EXT"))
      continue;

    if (arg.hasTag ("FILTER"))
    {
      auto canonical = arg.attribute ("canonical");
      auto raw       = arg.attribute ("raw");

      if (arg.hasTag ("HINT"))
      {
        if (expandIntervalHint (canonical, start, end))
        {
          args.push_back ("<date>");
          args.push_back ("-");
          args.push_back ("<date>");
        }

        // Hints that are not expandable to a date range are ignored.
      }
      else if (arg._lextype == Lexer::Type::date)
      {
        if (start == "")
          start = raw;
        else if (end == "")
          end = raw;

        args.push_back ("<date>");
      }
      else if (arg._lextype == Lexer::Type::duration)
      {
        if (duration == "")
          duration = raw;

        args.push_back ("<duration>");
      }
      else if (arg.hasTag ("KEYWORD"))
      {
        args.push_back (raw);
      }
      else
      {
        filter.tag (raw);
      }
    }
  }

  Range range;

  // <date>
  if (args.size () == 1 &&
      args[0] == "<date>")
  {
    range.start = Datetime (start);
    range.end   = Datetime ("now");
  }

  // from <date>
  else if (args.size () == 2 &&
           args[0] == "from" &&
           args[1] == "<date>")
  {
    range.start = Datetime (start);
    range.end   = Datetime ("now");
  }

  // <date> to/- <date>
  else if (args.size () == 3                   &&
           args[0] == "<date>"                 &&
           (args[1] == "to" || args[1] == "-") &&
           args[2] == "<date>")
  {
    range.start = Datetime (start);
    range.end   = Datetime (end);
  }

  // from/since <date> to/- <date>
  else if (args.size () == 4                   &&
           args[0] == "from"                   &&
           args[1] == "<date>"                 &&
           (args[2] == "to" || args[2] == "-") &&
           args[3] == "<date>")
  {
    range.start = Datetime (start);
    range.end   = Datetime (end);
  }

  // <date> for <duration>
  else if (args.size () == 3   &&
           args[0] == "<date>" &&
           args[1] == "for"    &&
           args[2] == "<duration>")
  {
    range.start = Datetime (start);
    range.end   = Datetime (start) + Duration (duration).toTime_t ();
  }

  // from/since <date> for <duration>
  else if (args.size () == 4                         &&
           (args[0] == "from" || args[0] == "since") &&
           args[1] == "<date>"                       &&
           args[2] == "for"                          &&
           args[3] == "<duration>")
  {
    range.start = Datetime (start);
    range.end   = Datetime (start) + Duration (duration).toTime_t ();
  }

  // <duration> before <date>
  else if (args.size () == 3 &&
           args[0] == "<duration>" &&
           args[1] == "before"     &&
           args[2] == "<date>")
  {
    range.start = Datetime (start) - Duration (duration).toTime_t ();
    range.end   = Datetime (start);
  }

  // <duration> after <date>
  else if (args.size () == 3 &&
           args[0] == "<duration>" &&
           args[1] == "after"      &&
           args[2] == "<date>")
  {
    range.start = Datetime (start);
    range.end   = Datetime (start) + Duration (duration).toTime_t ();
  }

  // <duration>
  else if (args.size () == 1 &&
           args[0] == "<duration>")
  {
    range.start = Datetime ("now") - Duration (duration).toTime_t ();
    range.end   = Datetime ("now");
  }

  // Unrecognized date range construct.
  else if (args.size ())
  {
    throw std::string ("Unrecognized date range: '") + join (" ", args) + "'.";
  }

  filter.range = range;
  return filter;
}

////////////////////////////////////////////////////////////////////////////////
// The five different overlap possibilities:
//
//               timeline.start      timeline.end
//               |                   |
// A [--------)  |                   |
// B        [----|----)              |
// C             |  [------------)   |
// D             |              [----|----)
// E             |                   |   [--------)
// F        [----|-------------------|----)
//               |                   |
//
// We really only need to eliminate A and F.
//
Timeline createTimelineFromData (
  const Rules& rules,
  Database& database,
  const Interval& filter)
{
  Timeline t;
  t.range = filter.range;

  // Add filtered intervals.
  for (auto& line : database.allLines ())
  {
    Interval i;
    i.initialize (line);
    if (intervalMatchesFilterInterval (i, filter))
      t.include (i);
  }

  // Add exclusions from configuration.
  for (auto& name : rules.all ("exclusions."))
    t.exclude (Exclusion (lowerCase (name), rules.get (name)));

  return t;
}

////////////////////////////////////////////////////////////////////////////////
Interval getLatestInterval (Database& database)
{
  Interval i;
  auto lastLine = database.lastLine ();
  if (lastLine != "")
  {
    i.initialize (lastLine);
    if (! i.range.ended ())
    {
      // TODO An open interval needs to be split against all exclusions.
    }
  }

  return i;
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter interval if the start/end overlaps, and all
// filter interval tags are found in the interval.
bool intervalMatchesFilterInterval (const Interval& interval, const Interval& filter)
{
  if ((filter.range.start.toEpoch () == 0 &&
       filter.range.end.toEpoch () == 0)

      ||

      ((interval.range.end.toEpoch () == 0       ||
        interval.range.end > filter.range.start) &&
       interval.range.start < filter.range.end))
  {
    for (auto& tag : filter.tags ())
      if (! interval.hasTag (tag))
        return false;

    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Compose a JSON document of intervals. In the trivial case:
//   [
//   ]
//
// In the non-trivial case:
//   [
//   {...},
//   {...}
//   ]
//
std::string jsonFromIntervals (const std::vector <Interval>& intervals)
{
  std::stringstream out;

  out << "[\n";
  int counter = 0;
  for (auto& interval : intervals)
  {
    if (counter)
      out << ",\n";

    out << interval.json ();
    ++counter;
  }

  if (counter)
    out << '\n';

  out << "]\n";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Read rules and extract all holiday definitions. Create a Range for each
// one that spans from midnight to midnight.
std::vector <Range> rangesFromHolidays (const Rules& rules)
{
  std::vector <Range> results;
  for (auto& holiday : rules.all ("holidays."))
  {
    auto lastDot = holiday.rfind ('.');
    if (lastDot != std::string::npos)
    {
      Range r;
      Datetime d (holiday.substr (lastDot + 1), "Y_M_D");
      r.start = d;
      ++d;
      r.end = d;
      results.push_back (r);
    }
  }

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// Subset both ranges and additions by limits, and combine.
std::vector <Range> addRanges (
  const Range& limits,
  const std::vector <Range>& ranges,
  const std::vector <Range>& additions)
{
  std::vector <Range> results;

  for (auto& range : ranges)
    if (limits.overlap (range))
      results.push_back (range);

  for (auto& addition : additions)
    if (limits.overlap (addition))
      results.push_back (addition);

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// Subtract a set of Range from another set of Range, all within a defined
// range.
std::vector <Range> subtractRanges (
  const Range& limits,
  const std::vector <Range>& ranges,
  const std::vector <Range>& subtractions)
{
  if (! subtractions.size ())
    return ranges;

  std::vector <Range> results;
  for (auto& r1 : ranges)
    for (auto& r2 : subtractions)
      for (auto& r3 : r1.subtract (r2))
        results.push_back (limits.intersect (r3));

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// From a set of intervals, find the earliest start and the latest end, and
// return these in a Range.
Range overallRangeFromIntervals (const std::vector <Interval>& intervals)
{
  Range overall;

  for (auto& interval : intervals)
  {
    if (interval.range.start < overall.start || overall.start.toEpoch () == 0)
      overall.start = interval.range.start;

    // Deliberately mixed start/end.
    if (interval.range.start > overall.end)
      overall.end = interval.range.start;

    if (interval.range.end > overall.end)
      overall.end = interval.range.end;
  }

  return overall;
}

////////////////////////////////////////////////////////////////////////////////
// [1] Read holiday definitions from the rules, extract their dates and create
//     a set of Range from them.
// [2] For 'exc day ...' exclusions, separate into daysOn and daysOff sets,
//     based on whether the exclusion is additive.
// [3] Treat daysOff as additional holidays.
// [4] Subtract daysOn from the set of holidays.
// [5] Take all the 'exc <dayOfWeek> ...' exclusions and expand them into
//     concrete ranges within the overall range, adding them to the results.
//
// The result is the complete set of untrackable time that lies within the
// input range. This will be a set of nights, weekends, holidays and lunchtimes.
std::vector <Range> combineHolidaysAndExclusions (
  const Range& range,
  const Rules& rules,
  const std::vector <Exclusion>& exclusions)
{
  // Start with the set of all holidays, intersected with range.
  std::vector <Range> results;
  results = addRanges (range, results, rangesFromHolidays (rules));

  // Find exclusions 'exc day on <date>' and remove from holidays.
  // Find exlcusions 'exc day off <date>' and add to holidays.
  std::vector <Range> daysOn;
  std::vector <Range> daysOff;
  for (auto& exclusion : exclusions)
  {
    if (exclusion.tokens ()[1] == "day")
    {
      if (exclusion.additive ())
        for (auto& r : exclusion.ranges (range))
          daysOn.push_back (r);
      else
        for (auto& r : exclusion.ranges (range))
          daysOff.push_back (r);
    }
  }

  // daysOff are combined with existing holidays.
  results = addRanges (range, results, daysOff);

  // daysOn are subtracted from the existing holidays.
  results = subtractRanges (range, results, daysOn);

  // Expand all exclusions that are not 'exc day ...' into excluded ranges that
  // overlage with range.
  std::vector <Range> exclusionRanges;
  for (auto& exclusion : exclusions)
    if (exclusion.tokens ()[1] != "day")
      for (auto& r : exclusion.ranges (range))
        exclusionRanges.push_back (r);

  return addRanges (range, results, exclusionRanges);
}

////////////////////////////////////////////////////////////////////////////////
Palette createPalette (const Rules& rules)
{
  Palette p;
  auto colors = rules.all ("theme.palette.color");
  if (colors.size ())
  {
    p.clear ();
    for (auto& c : colors)
      p.add (Color (rules.get (c)));
  }

  p.enabled = rules.getBoolean ("color");
  return p;
}

////////////////////////////////////////////////////////////////////////////////
// Extract the tags from a set of intervals, and using a rotating color palette,
// map unique tags to color.
std::map <std::string, Color> createTagColorMap (
  const Rules& rules,
  Palette& palette,
  const std::vector <Interval>& intervals)
{
  // TODO Note: tags may have their own defined color.

  std::map <std::string, Color> mapping;
  for (auto& interval : intervals)
    for (auto& tag : interval.tags ())
      if (mapping.find (tag) == mapping.end ())
        mapping[tag] = palette.next ();

  return mapping;
}

////////////////////////////////////////////////////////////////////////////////
int quantizeTo15Minutes (const int minutes)
{
  if (minutes == 0  ||
      minutes == 15 ||
      minutes == 45 ||
      minutes == 60)
    return minutes;

  auto deviation = minutes % 15;
  if (deviation < 8)
    return minutes - deviation;

  return minutes + 15 - deviation;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> splitInterval (
  const Interval& interval,
  const std::vector <Range>& exclusions)
{
  std::vector <Interval> all;

  // Start with a single range from the interval, from which to subtract.
  std::vector <Range> intervalFragments {interval.range};
  for (auto& exclusion : exclusions)
  {
    std::vector <Range> brokenFragments;
    for (auto& fragment : intervalFragments)
      for (auto& broken : fragment.subtract (exclusion))
        brokenFragments.push_back (broken);

    intervalFragments = brokenFragments;
  }

  // Return all the fragments as intervals.
  for (auto& fragment : intervalFragments)
  {
    // Clone the interval, set the new range.
    Interval clipped {interval};
    clipped.range = fragment;
    all.push_back (clipped);
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
