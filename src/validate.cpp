////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
  // An empty filter allows scanning beyond interval range.
  Interval range_filter;

  // Look backwards from interval.start to a boundary.
  auto tracked = getTracked (database, rules, range_filter);
  for (auto earlier = tracked.rbegin (); earlier != tracked.rend (); ++earlier)
  {
    if (! earlier->is_open () &&
        earlier->end <= interval.start)
    {
      interval.start = earlier->end;
        if (rules.getBoolean ("verbose"))
        std::cout << "Backfilled "
                  << (interval.id ? format ("@{1} ", interval.id) : "")
                  << "to "
                  << interval.start.toISOLocalExtended ()
                  << "\n";
      break;
    }
  }

// If the interval is closed, scan forwards for the next boundary.
  if (! interval.is_open ())
  {
    for (auto& later : tracked)
    {
      if (interval.end <= later.start)
      {
        interval.end = later.start;
        if (rules.getBoolean ("verbose"))
          std::cout << "Filled "
                    << (interval.id ? format ("@{1} ", interval.id) : "")
                    << "to "
                    << interval.end.toISOLocalExtended ()
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
  const bool verbose = rules.getBoolean ("verbose");

  Interval overlaps_filter {interval.start, interval.end};
  auto overlaps = getTracked (database, rules, overlaps_filter);

  if (overlaps.empty ())
  {
    return;
  }

  debug ("Input         " + interval.dump ());
  debug ("Overlaps with");

  for (auto& overlap : overlaps)
  {
    debug ("              " + overlap.dump ());
  }

  if (! adjust)
  {
    throw std::string ("You cannot overlap intervals. Correct the start/end time, or specify the :adjust hint.");
  }
  else
  {
    // implement overwrite resolution, i.e. the new interval overwrites existing intervals
    for (auto& overlap : overlaps)
    {
      bool start_within_overlap = interval.startsWithin (overlap);
      bool end_within_overlap = interval.endsWithin (overlap);

      if (start_within_overlap && !end_within_overlap)
      {
        // start date of new interval within old interval
        Interval modified {overlap};
        modified.end = interval.start;
        database.modifyInterval (overlap, modified, verbose);
      }
      else if (!start_within_overlap && end_within_overlap)
      {
        // end date of new interval within old interval
        Interval modified {overlap};
        modified.start = interval.end;
        database.modifyInterval (overlap, modified, verbose);
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

        split1.end = interval.start;
        split2.start = interval.end;

        if (split1.is_empty ())
        {
          database.deleteInterval (overlap);
        }
        else
        {
          database.modifyInterval (overlap, split1, verbose);
        }

        if (! split2.is_empty ())
        {
          database.addInterval (split2, verbose);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void validate (
  const CLI& cli,
  const Rules& rules,
  Database& database,
  Interval& interval)
{
  // All validation performed here.
  if (findHint (cli, ":fill"))
  {
    autoFill (rules, database, interval);
  }

  autoAdjust (findHint (cli, ":adjust"), rules, database, interval);
}

////////////////////////////////////////////////////////////////////////////////
