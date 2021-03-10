////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <algorithm>
#include <deque>

#include <cmake.h>
#include <shared.h>
#include <format.h>
#include <Datetime.h>
#include <Duration.h>
#include <timew.h>
#include <algorithm>
#include <iostream>
#include <IntervalFactory.h>

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

  debug (format ("Found {1} holidays", results.size ()));
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

  // Load all exclusions from configuration.
  std::vector <Exclusion> exclusions;
  for (auto& name : rules.all ("exclusions."))
    exclusions.push_back (Exclusion (lowerCase (name), rules.get (name)));
  debug (format ("Found {1} exclusions", exclusions.size ()));

  // Find exclusions 'exc day on <date>' and remove from holidays.
  // Find exclusions 'exc day off <date>' and add to holidays.
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
  if (! daysOn.empty ())
    debug (format ("Found {1} additional working days", daysOn.size ()));
  if (! daysOff.empty ())
    debug (format ("Found {1} additional non-working days", daysOff.size ()));

  // daysOn are subtracted from the existing holidays.
  results = subtractRanges (results, daysOn);

  // Expand all exclusions that are not 'exc day ...' into excluded ranges that
  // overlap with range.
  std::vector <Range> exclusionRanges;
  for (auto& exclusion : exclusions)
    if (exclusion.tokens ()[1] != "day")
      for (auto& r : exclusion.ranges (range))
        exclusionRanges.push_back (r);

  return merge (addRanges (range, results, exclusionRanges));
}

////////////////////////////////////////////////////////////////////////////////
void flattenDatabase (Database& database, const Rules& rules)
{
  auto latest = getLatestInterval (database);
  if (latest.is_open ())
  {
    auto exclusions = getAllExclusions (rules, {latest.start, Datetime ()});
    if (! exclusions.empty ())
    {
      Interval modified {latest};

      // Update database.
      database.deleteInterval (latest);
      for (auto& interval : flatten (modified, exclusions))
      {
        database.addInterval (interval, rules.getBoolean ("verbose"));
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// This function will return synthetic intervals
std::vector <Interval> getIntervalsByIds (
  Database& database,
  const Rules& rules,
  const std::set <int>& ids)
{
  std::vector <Interval> intervals;
  std::deque <Interval> synthetic;

  int current_id = 0;
  auto id_it = ids.begin ();
  auto id_end = ids.end ();

  auto it = database.begin ();
  auto end = database.end ();

  Interval latest;
  if (it != end )
  {
    latest = IntervalFactory::fromSerialization (*it);
  }

  // If the latest interval is open, check for synthetic intervals
  if (latest.is_open ())
  {
    auto exclusions = getAllExclusions (rules, {latest.start, Datetime ()});
    if (! exclusions.empty ())
    {
        // We're converting the latest interval into synthetic intervals so we need to skip it
        ++it;

        for (auto& interval : flatten (latest, exclusions))
        {
          ++current_id;
          interval.synthetic = true;
          interval.id = current_id;
          synthetic.push_front (interval);
        }
    }
  }

  intervals.reserve (ids.size ());

  // First look for the ids in our set of synthetic intervals
  for (auto& interval : synthetic)
  {
    if (id_it == id_end)
    {
      break;
    }

    if (interval.id == *id_it)
    {
      intervals.push_back (interval);
      ++id_it;
    }
  }

  current_id = synthetic.size ();

  // We'll find remaining intervals from the database itself
  for ( ; it != end; ++it)
  {
    ++current_id;

    if (id_it == id_end)
    {
      break;
    }

    if (current_id == *id_it)
    {
      Interval interval = IntervalFactory::fromSerialization (*it);
      interval.id = current_id;
      intervals.push_back (interval);
      ++id_it;
    }
  }

  // We did not find all the ids we were looking for.
  if (id_it != id_end)
  {
    throw format("ID '@{1}' does not correspond to any tracking.", *id_it);
  }

  return intervals;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> subset (
  const Interval& filter,
  const std::deque <Interval>& intervals)
{
  std::vector <Interval> all;
  for (auto& interval : intervals)
  {
    if (matchesFilter (interval, filter))
    {
      all.push_back (interval);
    }
  }

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

  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Range> subset (
  const Range& range,
  const std::vector <Range>& ranges)
{
  std::vector <Range> all;
  for (auto& r : ranges) {
    if (range.intersects (r)) {
      all.push_back (r);
    }
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> subset (
  const Range& range,
  const std::vector <Interval>& intervals)
{
  std::vector <Interval> all;
  for (auto& interval : intervals) {
    if (range.intersects (interval))
    {
      all.push_back (interval);
    }
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> flatten (
  const Interval& interval,
  const std::vector <Range>& exclusions)
{
  std::vector <Interval> all;

  std::vector <Range> enclosed;
  for (auto& e : exclusions)
    if (interval.encloses (e))
      enclosed.push_back (e);

  Datetime now;
  for (auto& result : subtractRanges ({interval}, enclosed))
  {
    if (interval.is_open() && result.start > now)
    {
      break;
    }

    Interval chunk {interval};
    chunk.setRange (result);

    all.push_back (chunk);
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
// Simply merges a vector of ranges, without data loss.
static bool rangeCompare (Range left, Range right)
{
  return left.start < right.start;
}

std::vector <Range> merge (
  const std::vector <Range>& ranges)
{
  // Short cut.
  if (ranges.size () < 2)
    return ranges;

  std::vector <Range> sorted {ranges};
  std::sort (sorted.begin (), sorted.end (), rangeCompare);

  unsigned int cursor = 0;
  int merges = 0;
  for (unsigned int i = 0; i < sorted.size (); ++i)
  {
    if (cursor && sorted[cursor - 1].overlaps (sorted[i]))
    {
      sorted[cursor - 1] = sorted[cursor - 1].combine (sorted[i]);
      ++merges;
    }
    else
    {
      sorted[cursor] = sorted[i];
      ++cursor;
    }
  }

  if (merges)
    sorted.resize (ranges.size () - merges);

  return sorted;
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
    if (limits.overlaps (range))
      results.push_back (range);

  for (auto& addition : additions)
    if (limits.overlaps (addition))
      results.push_back (addition);

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// Subtract a set of Ranges from another set of Ranges, all within a defined range.
std::vector <Range> subtractRanges (
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

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter range if the start/end overlaps
bool matchesRange (const Interval& interval, const Range& filter)
{
  return ((!filter.is_started() && !filter.is_ended()) || interval.intersects (filter));
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter interval if the start/end overlaps, and all
// filter interval tags are found in the interval.
//
// [1] interval.end.toEpoch () == 0
// [2] interval.end > filter.start
// [3] filter.end.toEpoch () == 0
// [4] interval.start < filter.end
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
  if (matchesRange (interval, filter))
  {
    for (auto& tag : filter.tags ())
    {
      if (! interval.hasTag (tag))
      {
        return false;
      }
    }
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Take an interval and clip it to the range
Interval clip (const Interval& interval, const Range& range)
{
  if (! range.is_started () ||
      range.total () == 0)
    return interval;

  Interval clipped {interval};
  if (clipped.start.toEpoch () &&
      clipped.start < range.start)
    clipped.start = range.start;

  if (clipped.end.toEpoch () &&
      clipped.end > range.end)
    clipped.end = range.end;

  return clipped;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> getTracked (
  Database& database,
  const Rules& rules,
  Interval& filter)
{
  int id_skip = 0;
  std::deque <Interval> intervals;

  for (auto& line : database)
  {
    Interval interval = IntervalFactory::fromSerialization(line);

    // Since we are moving backwards, and the intervals are in sorted order,
    // if the filter is after the interval, we know there will be no more matches
    if (matchesRange (interval, filter))
    {
      intervals.push_front (std::move (interval));
    }
    else if (interval.start.toEpoch () >= filter.start.toEpoch ())
    {
      ++id_skip;
    }
    else
    {
      break;
    }
  }

  if (! intervals.empty ())
  {
    const Datetime now {};
    auto latest = intervals.back ();

    if (latest.is_open () && latest.start <= now)
    {
      // Get the set of expanded exclusions that overlap the range defined by the open interval.
      Interval exclusion_range {};
      exclusion_range.start = latest.start;
      exclusion_range.end = now;

      auto exclusions = getAllExclusions (rules, exclusion_range);
      if (! exclusions.empty ())
      {
        intervals.pop_back ();

        for (auto& interval : flatten (latest, exclusions))
        {
          if (latest.synthetic ||
              latest != interval)
            interval.synthetic = true;

          intervals.push_back (interval);
        }
      }
    }
  }

  // Assign an ID to each interval before we prune ones that do not have matching tags
  for (unsigned int i = 0; i < intervals.size (); ++i)
  {
    intervals[i].id = intervals.size () - i + id_skip;
  }

  // Now prune the intervals without matching tags
  if (filter.tags ().size () > 0)
  {
    auto cmp = [&filter] (const Interval& interval)
               {
                 for (const auto& tag : filter.tags ())
                 {
                   if (! interval.hasTag (tag))
                   {
                     return true;
                   }
                 }
                 return false;
               };

    intervals.erase (std::remove_if (intervals.begin (), intervals.end (), cmp),
                     intervals.end ());
  }

  debug (format ("Loaded {1} tracked intervals", intervals.size ()));

  return std::vector <Interval> (std::make_move_iterator (intervals.begin ()),
                                 std::make_move_iterator (intervals.end ()));
}

////////////////////////////////////////////////////////////////////////////////
// Untracked time is that which is not excluded, and not filled. Gaps.
std::vector <Range> getUntracked (
  Database& database,
  const Rules& rules,
  Interval& filter)
{
  bool found_match = false;
  std::vector <Range> inclusion_ranges;
  for (auto& line : database)
  {
    Interval i = IntervalFactory::fromSerialization (line);
    if (matchesFilter (i, filter))
    {
      inclusion_ranges.push_back (i);
      found_match = true;
    }
    else if (found_match)
    {
      // If we already had a match, and now we do not, since the database is in
      // order from most recent to oldest inclusion, we can be sure that there
      // will not be any further matches.
      break;
    }
  }

  auto available = subtractRanges ({filter}, getAllExclusions (rules, filter));
  auto untracked = subtractRanges (available, inclusion_ranges);
  debug (format ("Loaded {1} untracked ranges", untracked.size ()));
  return untracked;
}

////////////////////////////////////////////////////////////////////////////////
Interval getLatestInterval (Database& database)
{
  Interval i;
  auto latestEntry = database.getLatestEntry ();
  if (! latestEntry.empty ())
  {
    i = IntervalFactory::fromSerialization (latestEntry);
    i.id = 1;
  }

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
                Datetime (y, m, d, 24, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////
