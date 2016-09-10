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
#include <format.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// :fill
//   The :fill hint is used to eliminate gaps on interval modification, and only
//   a single interval is affected.
//
//   Fill works by extending an interval in both directions if possible, to abut
//   either an interval or an exclusion, while being conћtrained by a filter
//   range.
//
void autoFill (
  const Rules& rules,
  Database& database,
  Interval& interval)
{
  // An empty filter allows scanning beyond interval.range.
  Interval range_filter;

  // Look backwards from interval.range.start to a boundary.
  auto tracked = getTracked (database, rules, range_filter);
  for (auto earlier = tracked.rbegin (); earlier != tracked.rend (); ++earlier)
  {
    if (! earlier->range.is_open () &&
        earlier->range.end < interval.range.start)
    {
      interval.range.start = earlier->range.end;
        if (rules.getBoolean ("verbose"))
        std::cout << "Backfilled "
                  << (interval.id ? format ("@{1} ", interval.id) : "")
                  << "to "
                  << interval.range.start.toISOLocalExtended ()
                  << "\n";
      break;
    }
  }

// If the interval is closed, scan forwards for the next boundary.
  if (! interval.range.is_open ())
  {
    for (auto& later : tracked)
    {
      if (interval.range.end < later.range.start)
      {
        interval.range.end = later.range.start;
        if (rules.getBoolean ("verbose"))
          std::cout << "Filled "
                    << (interval.id ? format ("@{1} ", interval.id) : "")
                    << "to "
                    << interval.range.end.toISOLocalExtended ()
                    << "\n";
        break;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// :adjust
//   To avoid disallowed overlaps, one or more intervals may be modified to
//   resolve the conflict according to configuration-based rules. Resolution
//   can involve rejection, adjustment of modified interval, or adjustment of
//   recorded data.
//
static void autoAdjust (
  bool adjust,
  const Rules& rules,
  Database& database,
  Interval& interval)
{
  // Without (adjust == true), overlapping intervals are an error condition.

  // An empty filter allows scanning beyond interval.range.
  Interval range_filter;
  auto tracked = getTracked (database, rules, range_filter);

  // Find all overlapping intervals.
  std::vector <Interval> overlapping;
  for (auto& track : tracked)
    if (interval.range.overlap (track.range))
      overlapping.push_back (track);

  // Diagnostics.
  debug ("Input         " + interval.dump ());
  debug ("Overlaps with");
  for (auto& overlap : overlapping)
    debug ("             " + overlap.dump ());

  // Overlaps are forbidden.
  if (! adjust && overlapping.size ())
    throw std::string ("You cannot overlap intervals. Correct the start/end "
                       "time, or specify the :adjust hint.");

  // TODO Accumulate identifiable and correctable cases here.
/*

  ext      [-----]
  new   [-----]

  ext   [-----]
  new      [-----]

  ext      [-]
  new   [-----]

  ext   [-----]
  new      [-]

  ext   [-----]
  new   [-----]

  ext      [-----]
  new   [--

  ext   [-----]
  new      [--

  ext      [-]
  new   [--

  ext   [-----]
  new      [-

  ext   [-----]
  new   [--

*/

}

////////////////////////////////////////////////////////////////////////////////
// Warn on new tag.
static void warnOnNewTag (
  const Rules& rules,
  Database& database,
  const Interval& interval)
{
  if (rules.getBoolean ("verbose"))
  {
    std::set <std::string> tags;
    for (auto& line : database.allLines ())
    {
      if (line[0] == 'i')
      {
        Interval interval;
        interval.initialize (line);

        for (auto& tag : interval.tags ())
          tags.insert (tag);
      }
    }

    for (auto& tag : interval.tags ())
      if (tags.find (tag) == tags.end ())
        std::cout << "Note: '" << tag << "' is a new tag.\n";
  }
}

////////////////////////////////////////////////////////////////////////////////
void validate (
  const CLI& cli,
  const Rules& rules,
  Database& database,
  Interval& interval)
{
  // Create a filter, and if empty, choose 'today'.
  auto filter = getFilter (cli);
  if (! filter.range.is_started ())
    filter.range = Range (Datetime ("today"), Datetime ("tomorrow"));

  // All validation performed here.
  if (findHint (cli, ":fill"))
    autoFill (rules, database, interval);

  autoAdjust (findHint (cli, ":adjust"), rules, database, interval);

  // TODO This warning is not working properly, because when an interval is
  //      modified, it ifirst deleted, then added. This causes this code to
  //      dertmine that it is always a new tag.
  //warnOnNewTag (rules, database, interval);
}

////////////////////////////////////////////////////////////////////////////////
