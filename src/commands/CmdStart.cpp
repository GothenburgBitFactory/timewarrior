////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <commands.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdStart (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Add a new open interval, which may have a defined start time.
  auto filter = getFilter (cli);
  auto latest = getLatestInterval (database);

  database.startTransaction ();

  // If the latest interval is open, close it.
  if (latest.range.is_open ())
  {
    // If the new interval tags match those of the currently open interval, then
    // do nothing - the tags are already being tracked.
    if (latest.tags () == filter.tags ())
    {
      if (rules.getBoolean ("verbose"))
        std::cout << intervalSummarize (database, rules, latest);

      return 0;
    }

    // Stop it, at the given start time, if applicable.
    Interval modified {latest};
    if (filter.range.start.toEpoch () != 0)
      modified.range.end = filter.range.start;
    else
      modified.range.end = Datetime ();

    // Update database.
    database.deleteInterval (latest);
    validate (cli, rules, database, modified);

    for (auto& interval : flatten (modified, getAllExclusions (rules, modified.range)))
    {
      database.addInterval (interval, rules.getBoolean ("verbose"));

      if (rules.getBoolean ("verbose"))
        std::cout << intervalSummarize (database, rules, interval);
    }
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
  validate (cli, rules, database, now);
  database.addInterval (now, rules.getBoolean ("verbose"));

  if (rules.getBoolean ("verbose"))
    std::cout << intervalSummarize (database, rules, now);

  database.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
