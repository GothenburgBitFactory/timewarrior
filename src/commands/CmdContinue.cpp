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
#include <format.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdContinue (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Gather IDs and TAGs.
  std::set <int> ids = cli.getIds();

  if (ids.size() > 1)
    throw std::string ("You can only specify one ID to continue.");

  Interval to_copy;
  Interval latest = getLatestInterval (database);

  if (ids.size() == 1)
  {
    // Load the data.
    // Note: There is no filter.
    Interval filter;
    auto tracked = getTracked (database, rules, filter);

    auto id = *ids.begin ();

    if (id > static_cast <int> (tracked.size ()))
      throw format ("ID '@{1}' does not correspond to any tracking.", id);

    to_copy = tracked[tracked.size () - id];
  }
  else
  {
    if (latest.empty ())
      throw std::string ("There is no previous tracking to continue.");

    if (latest.range.is_open ())
      throw std::string ("There is already active tracking.");

    to_copy = latest;
  }

  auto filter = getFilter (cli);
  Datetime start_time;
  Datetime end_time;

  database.startTransaction ();

  if (filter.range.start.toEpoch () != 0)
  {
    start_time = filter.range.start;
    end_time = filter.range.end;
  }
  else
  {
    start_time = Datetime ();
    end_time = 0;
  }

  if (latest.range.is_open ())
  {
    auto exclusions = getAllExclusions (rules, filter.range);

    // Stop it, at the given start time, if applicable.
    Interval modified {latest};
    modified.range.end = start_time;

    // Update database.
    database.deleteInterval (latest);
    for (auto& interval : flatten (modified, exclusions))
    {
      database.addInterval (interval, rules.getBoolean ("verbose"));

      if (rules.getBoolean ("verbose"))
        std::cout << '\n' << intervalSummarize (database, rules, interval);
    }
  }

  // Create an identical interval and update the DB.
  to_copy.range.start = start_time;
  to_copy.range.end = end_time;

  validate (cli, rules, database, to_copy);
  database.addInterval (to_copy, rules.getBoolean ("verbose"));

  database.endTransaction ();

  if (rules.getBoolean ("verbose"))
    std::cout << intervalSummarize (database, rules, to_copy);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
