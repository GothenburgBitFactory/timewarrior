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
#include <shared.h>
#include <Datetime.h>
#include <Duration.h>
#include <timew.h>
/*
#include <iostream> // TODO Remove.
*/

////////////////////////////////////////////////////////////////////////////////
// A filter is just another interval, containing start, end and tags.
//
// Supported interval forms:
//   ["from"] <date> ["to"|"-" <date>]
//   ["from"] <date> "for" <duration>
//   <duration> ["before"|"after" <date>]
//   <duration> "ago"
//
Interval getFilter (const CLI& cli)
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
        // Note: that KEYWORDS are not entities (why not?) and there is a list
        //       in CLI.cpp of them that must be maintained and synced with this
        //       function.
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
    range.end   = Datetime ();
  }

  // from <date>
  else if (args.size () == 2 &&
           args[0] == "from" &&
           args[1] == "<date>")
  {
    range.start = Datetime (start);
    range.end   = Datetime ();
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
  else if (args.size () == 4       &&
           args[0] == "from"       &&
           args[1] == "<date>"     &&
           args[2] == "for"        &&
           args[3] == "<duration>")
  {
    range.start = Datetime (start);
    range.end   = Datetime (start) + Duration (duration).toTime_t ();
  }

  // <duration> before <date>
  else if (args.size () == 3       &&
           args[0] == "<duration>" &&
           args[1] == "before"     &&
           args[2] == "<date>")
  {
    range.start = Datetime (start) - Duration (duration).toTime_t ();
    range.end   = Datetime (start);
  }

  // <duration> after <date>
  else if (args.size () == 3       &&
           args[0] == "<duration>" &&
           args[1] == "after"      &&
           args[2] == "<date>")
  {
    range.start = Datetime (start);
    range.end   = Datetime (start) + Duration (duration).toTime_t ();
  }

  // <duration> ago
  else if (args.size () == 2       &&
           args[0] == "<duration>" &&
           args[1] == "ago")
  {
    range.start = Datetime () - Duration (duration).toTime_t ();
    range.end   = Datetime ();
  }

  // for <duration>
  else if (args.size () == 2       &&
           args[0] == "for"        &&
           args[1] == "<duration>")
  {
    range.start = Datetime () - Duration (duration).toTime_t ();
    range.end   = Datetime ();
  }

  // <duration>
  else if (args.size () == 1 &&
           args[0] == "<duration>")
  {
    range.start = Datetime () - Duration (duration).toTime_t ();
    range.end   = Datetime ();
  }

  // Unrecognized date range construct.
  else if (args.size ())
  {
    throw std::string ("Unrecognized date range: '") + join (" ", args) + "'.";
  }

  filter.range = range;
/*
  std::cout << "# getFilter:\n"
            << "#   " << filter.dump () << "\n";
*/
  return filter;
}

////////////////////////////////////////////////////////////////////////////////
// Read rules and extract all holiday definitions. Create a Range for each
// one that spans from midnight to midnight.
std::vector <Range> getHolidays (const Rules& rules)
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

/*
  std::cout << "# getHolidays:\n";
  for (auto& h : results)
    std::cout << "#   " << h.dump () << "\n";
*/
  return results;
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
std::vector <Range> getAllExclusions (
  const Rules& rules,
  const Range& range)
{
  // Start with the set of all holidays, intersected with range.
  std::vector <Range> results;
  results = addRanges (range, results, getHolidays (rules));

  auto exclusions = getExclusions (rules);

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

/*
  auto all = addRanges (range, results, exclusionRanges);
  std::cout << "# getAllExclusions:\n";
  for (auto& r : all)
    std::cout << "#   " << r.dump () << "\n";
  return all;
*/
  return addRanges (range, results, exclusionRanges);
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Exclusion> getExclusions (const Rules& rules)
{
  // Add exclusions from configuration.
  std::vector <Exclusion> all;
  for (auto& name : rules.all ("exclusions."))
    all.push_back (Exclusion (lowerCase (name), rules.get (name)));

/*
  std::cout << "# getExclusions:\n";
  for (auto& e : all)
    std::cout << "#   " << e.dump () << "\n";
*/
  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> getAllInclusions (Database& database)
{
  std::vector <Interval> all;
  for (auto& line : database.allLines ())
  {
    Interval i;
    i.initialize (line);
    all.push_back (i);
  }

/*
  std::cout << "# getAllInclusions:\n";
  for (auto& i : all)
    std::cout << "#   " << i.dump () << "\n";
*/
  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> subset (
  const Interval& filter,
  const std::vector <Interval>& intervals)
{
  std::vector <Interval> all;
  for (auto& interval : intervals)
    if (matchesFilter (interval, filter))
      all.push_back (interval);

/*
  std::cout << "# subset (filter intervals):\n";
  for (auto& i : all)
    std::cout << "#   " << i.dump () << "\n";
*/
  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Range> subset (
  const Range& range,
  const std::vector <Range>& ranges)
{
  std::vector <Range> all;
  for (auto& r : ranges)
    if (range.overlap (r))
      all.push_back (r);

/*
  std::cout << "# subset (ranges):\n";
  for (auto& r : all)
    std::cout << "#   " << r.dump () << "\n";
*/
  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> subset (
  const Range& range,
  const std::vector <Interval>& intervals)
{
  std::vector <Interval> all;
  for (auto& interval : intervals)
    if (range.overlap (interval.range))
      all.push_back (interval);

/*
  std::cout << "# subset (intervals):\n";
  for (auto& i : all)
    std::cout << "#   " << i.dump () << "\n";
*/
  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> flatten (
  const Interval& interval,
  const std::vector <Range>& exclusions)
{
  std::vector <Interval> all;

  if (! interval.range.ended ())
  {
    all.push_back (interval);
  }
  else
  {
    for (auto& result : subtractRanges (interval.range, {interval.range}, exclusions))
    {
      Interval chunk {interval};
      chunk.range = result;
      all.push_back (chunk);
    }
  }

/*
  std::cout << "#   results:\n";
  for (auto& i : all)
    std::cout << "#     " << i.dump () << "\n";
*/
  return all;
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

/*
  std::cout << "# addRange:\n";
  for (auto& result : results)
    std::cout << "#   " << result.dump () << "\n";
*/
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
  std::vector <Range> results = ranges;
  for (auto& s : subtractions)
  {
    std::vector <Range> split_results;
    for (auto& range : results)
      for (auto& split_range : range.subtract (s))
        split_results.push_back (split_range);

    results = split_results;
  }

/*
  std::cout << "# subtractRange:\n";
  for (auto& result : results)
    std::cout << "#   " << result.dump () << "\n";
*/
  return results;
}

////////////////////////////////////////////////////////////////////////////////
// From a set of intervals, find the earliest start and the latest end, and
// return these in a Range.
Range outerRange (const std::vector <Interval>& intervals)
{
  Range outer;
  for (auto& interval : intervals)
  {
    if (interval.range.start < outer.start || outer.start.toEpoch () == 0)
      outer.start = interval.range.start;

    // Deliberately mixed start/end.
    if (interval.range.start > outer.end)
      outer.end = interval.range.start;

    if (interval.range.end > outer.end)
      outer.end = interval.range.end;

    if (! interval.range.ended ())
      outer.end = Datetime ();
  }

/*
  std::cout << "# outerRange " << outer.dump () << "\n";
*/
  return outer;
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter interval if the start/end overlaps, and all
// filter interval tags are found in the interval.
//
// [1] interval.range.end.toEpoch () == 0
// [2] interval.range.end > filter.range.start
// [3] filter.range.end.toEpoch () == 0
// [4] interval.range.start < filter.range.end
//
// Match:   (1 || 2) && (3 || 4)

// filter closed         [--------)                  1  2  3  4  5  6  result
// --------------------------------------------------------------------------
// A       [--------)    .        .                  0  0  0  1  0  0       0
// B                [--------)    .                  0  1  0  1  1  1       1
// C                     . [----) .                  0  1  0  1  1  1       1
// D                     .    [--------)             0  1  0  1  1  1       1
// E                     .        .    [--------)    0  1  0  0  1  0       0
// F                   [-------------)               0  1  0  1  1  1       1
// G                   [...       .                  1  0  0  1  1  1       1
// H                     .  [...  .                  1  0  0  1  1  1       1
// I                     .        . [...             1  0  0  0  1  0       0
//
//
//
// filter open           [...                        1  2  3  4  5  6  result
// --------------------------------------------------------------------------
// A       [--------)    .                           0  0  1  0  0  1       0
// B                [--------)                       0  1  1  0  1  1       1
// C                     . [----)                    0  1  1  0  1  1       1
// D                     .    [--------)             0  1  1  0  1  1       1
// E                     .             [--------)    0  1  1  0  1  1       1
// F                   [-------------)               0  1  1  0  1  1       1
// G                   [...                          1  0  1  0  1  1       1
// H                     .  [...                     1  0  1  0  1  1       1
// I                     .          [...             1  0  1  0  1  1       1
//
bool matchesFilter (const Interval& interval, const Interval& filter)
{
  if ((filter.range.start.toEpoch () == 0 &&
       filter.range.end.toEpoch () == 0)

      ||

      ((interval.range.end.toEpoch () == 0 || interval.range.end   > filter.range.start) &&
       (filter.range.end.toEpoch ()   == 0 || interval.range.start < filter.range.end)))
  {
    for (auto& tag : filter.tags ())
      if (! interval.hasTag (tag))
        return false;

    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Take an interval and clip it to the range.
Interval clip (const Interval& interval, const Range& range)
{
  if (! range.started () ||
      range.total () == 0)
    return interval;

  Interval clipped {interval};
  if (clipped.range.start.toEpoch () &&
      clipped.range.start < range.start)
    clipped.range.start = range.start;

  if (clipped.range.end.toEpoch () &&
      clipped.range.end > range.end)
    clipped.range.end = range.end;

/*
  std::cout << "# clip:\n"
            << "#   input   " << interval.dump () << "\n"
            << "#   range   " << range.dump () << "\n"
            << "#   clipped " << clipped.dump () << "\n";
*/
  return clipped;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> getTrackedIntervals (
  Database& database,
  const Rules& rules,
  Interval& filter)
{
  auto inclusions = getAllInclusions (database);

  // Exclusions are only usable within a range, so if no filter range exists,
  // determine the outermost range of the inclusions, ie:
  //
  //   [earliest start, latest end)
  //
  // Avoid assigning a zero-width range - leave it unstarted instead.
  if (! filter.range.started ())
  {
    auto outer = outerRange (inclusions);
    if (outer.total ())
      filter.range = outer;
  }

  // Get the set of expanded exclusions that overlap the range defined by the
  // timeline. If no range is defined, derive it from the set of all data.
  auto exclusions = getAllExclusions (rules, filter.range);

  std::vector <Interval> intervals;
  for (auto& inclusion : subset (filter, inclusions))
    for (auto& interval : flatten (clip (inclusion, filter.range), exclusions))
      intervals.push_back (interval);

  return intervals;
}

////////////////////////////////////////////////////////////////////////////////
// Untracked time is that which is not excluded, and not filled. Gaps.
std::vector <Range> getUntrackedRanges (
  Database& database,
  const Rules& rules,
  Interval& filter)
{
  std::vector <Range> available {filter.range};
  available = subtractRanges (filter.range, available, getAllExclusions (rules, filter.range));

  std::vector <Range> inclusion_ranges;
  for (auto& i : getAllInclusions (database))
    inclusion_ranges.push_back (i.range);

  available = subtractRanges (filter.range, available, inclusion_ranges);

/*
  std::cout << "# After subtracting exclusions, inclusions:\n";
  for (auto& a : available)
    std::cout << "#   " << a.dump () << "\n";
*/

  return available;
}

////////////////////////////////////////////////////////////////////////////////
Interval getLatestInterval (Database& database)
{
  Interval i;
  for (auto& line : database.allLines ())
  {
    // inc YYYYMMDDTHHMMSSZ - YYYYMMDDTHHMMSSZ # ...
    //                     ^ 20
    auto separator = line.find (" - ");
    if (separator != std::string::npos &&
        separator != 20)
    {
      i.initialize (line);
      return i;
    }
  }

  auto lastLine = database.lastLine ();
  if (lastLine != "")
    i.initialize (lastLine);

  return i;
}

////////////////////////////////////////////////////////////////////////////////
Range getFullDay (const Datetime& day)
{
  int y;
  int m;
  int d;
  day.toYMD (y, m, d);
  return Range (Datetime (y, m, d, 0, 0, 0),
                Datetime (y, m, d, 23, 59, 59));
}

////////////////////////////////////////////////////////////////////////////////
