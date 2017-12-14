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
//   Fill works by extending an interval in both directions if possible, to
//   about either an interval or an exclusion, while being constrained by a
//   filter range.
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
        earlier->range.end <= interval.range.start)
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
      if (interval.range.end <= later.range.start)
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
  auto overlaps = getOverlaps (database, rules, interval);
  debug ("Input         " + interval.dump ());
  debug ("Overlaps with");
  for (auto& overlap : overlaps)
    debug ("              " + overlap.dump ());

  if (! overlaps.empty ())
  {
    if (! adjust)
      throw std::string("You cannot overlap intervals. Correct the start/end "
                          "time, or specify the :adjust hint.");

    // implement overwrite resolution, i.e. the new interval overwrites existing intervals
    for (auto& overlap : overlaps)
    {
      bool start_within_overlap = overlap.range.contains (interval.range.start);
      bool end_within_overlap = interval.range.end != 0 && overlap.range.contains (interval.range.end);

      if (start_within_overlap && !end_within_overlap)
      {
        // start date of new interval within old interval
        Interval modified {overlap};
        modified.range.end = interval.range.start;
        database.modifyInterval (overlap, modified);
      }
      else if (!start_within_overlap && end_within_overlap)
      {
        // end date of new interval within old interval
        Interval modified {overlap};
        modified.range.start = interval.range.end;
        database.modifyInterval (overlap, modified);
      }
      else if (!start_within_overlap && !end_within_overlap)
      {
        // new interval encloses old interval
        database.deleteInterval (overlap);
      }
      else
      {
        // new interval enclosed by old interval
        Interval split2 {overlap};
        Interval split1 {overlap};

        split1.range.end = interval.range.start;
        split2.range.start = interval.range.end;

        if (split1.range.is_empty ())
        {
          database.deleteInterval (overlap);
        }
        else
        {
          database.modifyInterval (overlap, split1);
        }

        if (! split2.range.is_empty ())
        {
          database.addInterval (split2);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Warn on new tag.
static void warnOnNewTag (
  const Rules&,
  Database&,
  const Interval&)
{
  // TODO This warning is not working properly, because when an interval is
  //      modified, it is first deleted, then added. This causes this code to
  //      determine that it is always a new tag.
/*
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
*/
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

  warnOnNewTag (rules, database, interval);
}

////////////////////////////////////////////////////////////////////////////////
