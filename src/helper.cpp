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
Color tagColor (const Rules& rules, const std::string& tag)
{
  Color c;
  std::string name = std::string ("tag.") + tag + ".color";
  if (rules.has (name))
    c = Color (rules.get (name));

  return c;
}

////////////////////////////////////////////////////////////////////////////////
std::string intervalSummarize (const Rules& rules, const Interval& interval)
{
  std::stringstream out;
  if (interval.isStarted ())
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
    if (interval.isEnded ())
    {
      Duration dur (Datetime (interval.end ()) - Datetime (interval.start ()));
      out << "Recorded " << tags << "\n"
          << "  Started " << interval.start ().toISOLocalExtended () << "\n"
          << "  Ended   " << interval.end ().toISOLocalExtended () << "\n"
          << "  Elapsed " << std::setw (19) << std::setfill (' ') << dur.format () << "\n";
    }

    // Interval open.
    else
    {
      Duration dur (Datetime () - interval.start ());
      out << "Tracking " << tags << "\n"
          << "  Started " << interval.start ().toISOLocalExtended () << "\n";

      if (dur.toTime_t () > 10)
        out << "  Elapsed " << std::setw (19) << std::setfill (' ') << dur.format () << "\n";
    }
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
bool expandIntervalHint (
  const std::string& hint,
  std::string& start,
  std::string& end)
{
  static std::map <std::string, std::vector <std::string>> hints
  {
    {":day",     {"today", "tomorrow"}},
    {":week",    {"socw",  "eocw"}},
    {":month",   {"socw",  "eocw"}},
    {":quarter", {"socw",  "eocw"}},
    {":year",    {"socw",  "eocw"}},
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
// A filter is a placeholder for a start datetime, end datetime and a set of
// tags, which makes it essentially an interval.
//
// Supported interval forms:
//   ["from"] <date> ["to"|"-" <date>]
//   ["from"] <date> "for" <duration>
//   <duration> ["before"|"after" <date>]
//
Filter createFilterFromCLI (const CLI& cli)
{
  Filter filter;
  std::string start;
  std::string end;
  std::string duration;

  std::vector <std::string> args;
  for (auto& arg : cli._args)
  {
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

  Daterange range;

  // <date>
  if (args.size () == 1 &&
      args[0] == "<date>")
  {
    range.start (Datetime (start));
    range.end (Datetime ("now"));
  }

  // from <date>
  else if (args.size () == 2 &&
           args[0] == "from" &&
           args[1] == "<date>")
  {
    range.start (Datetime (start));
    range.end (Datetime ("now"));
  }

  // <date> to/- <date>
  else if (args.size () == 3                   &&
           args[0] == "<date>"                 &&
           (args[1] == "to" || args[1] == "-") &&
           args[2] == "<date>")
  {
    range.start (Datetime (start));
    range.end (Datetime (end));
  }

  // from/since <date> to/- <date>
  else if (args.size () == 4                   &&
           args[0] == "from"                   &&
           args[1] == "<date>"                 &&
           (args[2] == "to" || args[2] == "-") &&
           args[3] == "<date>")
  {
    range.start (Datetime (start));
    range.end (Datetime (end));
  }

  // <date> for <duration>
  else if (args.size () == 3   &&
           args[0] == "<date>" &&
           args[1] == "for"    &&
           args[2] == "<duration>")
  {
    range.start (Datetime (start));
    range.end (Datetime (start) + Duration (duration).toTime_t ());
  }

  // from/since <date> for <duration>
  else if (args.size () == 4                         &&
           (args[0] == "from" || args[0] == "since") &&
           args[1] == "<date>"                       &&
           args[2] == "for"                          &&
           args[3] == "<duration>")
  {
    range.start (Datetime (start));
    range.end (Datetime (start) + Duration (duration).toTime_t ());
  }

  // <duration> before <date>
  else if (args.size () == 3 &&
           args[0] == "<duration>" &&
           args[1] == "before"     &&
           args[2] == "<date>")
  {
    range.start (Datetime (start) - Duration (duration).toTime_t ());
    range.end (Datetime (start));
  }

  // <duration> after <date>
  else if (args.size () == 3 &&
           args[0] == "<duration>" &&
           args[1] == "after"      &&
           args[2] == "<date>")
  {
    range.start (Datetime (start));
    range.end (Datetime (start) + Duration (duration).toTime_t ());
  }

  // <duration>
  else if (args.size () == 1 &&
           args[0] == "<duration>")
  {
    range.start (Datetime ("now") - Duration (duration).toTime_t ());
    range.end (Datetime ("now"));
  }

  // Unrecognized date range construct.
  else if (args.size ())
  {
    throw std::string ("Unrecognized date range: '") + join (" ", args) + "'.";
  }

  filter.range (range);
  return filter;
}

////////////////////////////////////////////////////////////////////////////////
Interval createIntervalFromFilter (const Filter& filter)
{
  Interval interval;
  interval.range (filter.range ());

  for (auto& tag : filter.tags ())
    interval.tag (tag);

  return interval;
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
  const Filter& filter)
{
  Timeline t;
  t.range (filter.range ());

  // Add filtered intervals.
  for (auto& line : database.allLines ())
  {
    if (line[0] == 'i')
    {
      Interval i;
      i.initialize (line);

      if (intervalMatchesFilter (i, filter))
        t.include (i);
    }
    else if (line[0] == 'e')
    {
      // Exclusions are not filtered, so they all match.  This makes sure that
      // the correct exclusions are lined up ahead of the intervals.
      Exclusion e;
      e.initialize (line);
      t.exclude (e);
    }
  }

  return t;
}

////////////////////////////////////////////////////////////////////////////////
Interval getLatestInterval (Database& database)
{
  Interval i;
  auto lastLine = database.lastLine ();
  if (lastLine != "")
    i.initialize (lastLine);

  // TODO Mask i against timeline.

  return i;
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter if the start/end overlaps, and all filter tags
// are found in the interval.
bool intervalMatchesFilter (const Interval& interval, const Filter& filter)
{
  if ((filter.range ().start ().toEpoch () == 0 &&
       filter.range ().end ().toEpoch () == 0)

      ||

      (interval.end () > filter.range ().start () &&
       interval.start () < filter.range ().end ()))
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
    out << "\n";

  out << "]\n";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Daterange> rangesFromHolidays (const Rules& rules)
{
  std::vector <Daterange> results;
  for (auto& holiday : rules.all ("holidays."))
  {
    auto lastDot = holiday.rfind ('.');
    if (lastDot != std::string::npos)
    {
      Daterange r;
      Datetime d (holiday.substr (lastDot + 1), "Y_M_D");
      r.start (d);
      ++d;
      r.end (d);
      results.push_back (r);
    }
  }

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// Subset both ranges and additions by limits, and combine.
std::vector <Daterange> addRanges (
  const Daterange& limits,
  const std::vector <Daterange>& ranges,
  const std::vector <Daterange>& additions)
{
  std::vector <Daterange> results;

  for (auto& range : ranges)
    if (limits.overlap (range))
      results.push_back (range);

  for (auto& addition : additions)
    if (limits.overlap (addition))
      results.push_back (addition);

  return results;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Daterange> subtractRanges (
  const Daterange& limits,
  const std::vector <Daterange>& ranges,
  const std::vector <Daterange>& subtractions)
{
  if (! subtractions.size ())
    return ranges;

  std::vector <Daterange> results;
  for (auto& r1 : ranges)
    for (auto& r2 : subtractions)
      for (auto& r3 : r1.subtract (r2))
        results.push_back (limits.intersect (r3));

  return results;
}

////////////////////////////////////////////////////////////////////////////////
Daterange overallRangeFromIntervals (const std::vector <Interval>& intervals)
{
  Daterange overall;

  for (auto& interval : intervals)
  {
    if (interval.start () < overall.start () || overall.start ().toEpoch () == 0)
      overall.start (interval.start ());

    // Deliberately mixed start/end.
    if (interval.start () > overall.end ())
      overall.end (interval.start ());

    if (interval.end () > overall.end ())
      overall.end (interval.end ());
  }

  return overall;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Daterange> combineHolidaysAndExclusions (
  const Daterange& range,
  const Rules& rules,
  const std::vector <Exclusion>& exclusions)
{
  // Start with the set of all holidays, intersected with range.
  std::vector <Daterange> results;
  results = addRanges (range, results, rangesFromHolidays (rules));

  // Find exclusions 'exc day on <date>' and remove from holidays.
  // Find exlcusions 'exc day off <date>' and add to holidays.
  std::vector <Daterange> daysOn;
  std::vector <Daterange> daysOff;
  for (auto& exclusion : exclusions)
  {
    if (exclusion.tokens ()[1] == "day")
    {
      if (exclusion.additive ())
        for (auto& range : exclusion.ranges (range))
          daysOn.push_back (range);
      else
        for (auto& range : exclusion.ranges (range))
          daysOff.push_back (range);
    }
  }

  // daysOff are combined with existing holidays.
  results = addRanges (range, results, daysOff);

  // daysOn are subtracted from the existing holidays.
  results = subtractRanges (range, results, daysOn);

  // Expand all exclusions that are not 'exc day ...' into excluded ranges that
  // overlage with range.
  std::vector <Daterange> exclusionRanges;
  for (auto& exclusion : exclusions)
    if (exclusion.tokens ()[1] != "day")
      for (auto& range : exclusion.ranges (range))
        exclusionRanges.push_back (range);

  results = addRanges (range, results, exclusionRanges);
  return results;
}

////////////////////////////////////////////////////////////////////////////////
