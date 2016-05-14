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
#include <commands.h>
#include <timew.h>
#include <Interval.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdStart (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Add a new open interval, which may have a defined start time.
  auto filter     = getFilter (cli);
  auto holidays   = subset (filter.range, getHolidays (rules));
  auto exclusions = getAllExclusions (rules, filter.range);

  // If the latest interval is open, close it.
  auto latest = getLatestInterval (database);
  if (latest.range.is_open ())
  {
    // Stop it.
    Interval modified {latest};
    modified.range.end = Datetime ();

    // Update database.
    database.deleteInterval (latest);
    for (auto& interval : flatten (modified, exclusions))
      database.addInterval (interval);

    // TODO intervalSummarїze needs to operate on a vector of similar intervals.
    if (rules.getBoolean ("verbose"))
      std::cout << intervalSummarize (database, rules, modified);
  }

  // Now add the new open interval.
  Interval now;
  if (filter.range.start.toEpoch () != 0)
    now.range.start = filter.range.start;
  else
    now.range.start = Datetime ();

  for (auto& tag : filter.tags ())
    now.tag (tag);

  // Update database. An open interval does not need to be flattened.
  database.addInterval (now);

  // TODO intervalSummarїze needs to operate on a vector of similar intervals.
  // User feedback.
  if (rules.getBoolean ("verbose"))
    std::cout << intervalSummarize (database, rules, now);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
