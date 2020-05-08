////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
int CmdContinue (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  const bool verbose = rules.getBoolean ("verbose");

  // Gather IDs and TAGs.
  std::set <int> ids = cli.getIds();

  if (ids.size() > 1)
  {
    throw std::string ("You can only specify one ID to continue.");
  }

  journal.startTransaction ();

  flattenDatabase (database, rules);

  Interval to_copy;
  Interval latest = getLatestInterval (database);

  if (ids.size() == 1)
  {
    auto intervals = getIntervalsByIds (database, rules, ids);

    if (intervals.size () == 0)
    {
      throw format ("ID '@{1}' does not correspond to any tracking.", *ids.begin ());
    }

    assert (intervals.size () == 1);
    to_copy = intervals.front ();
  }
  else
  {
    if (latest.empty ())
    {
      throw std::string ("There is no previous tracking to continue.");
    }

    if (latest.is_open ())
    {
      throw std::string ("There is already active tracking.");
    }

    to_copy = latest;
  }

  auto filter = cli.getFilter ();
  Datetime start_time;
  Datetime end_time;

  if (filter.start.toEpoch () != 0)
  {
    start_time = filter.start;
    end_time = filter.end;
  }
  else
  {
    start_time = Datetime ();
    end_time = 0;
  }

  // Create an identical interval and update the DB.
  to_copy.start = start_time;
  to_copy.end = end_time;

  if (latest.is_open ())
  {
    Interval modified {latest};
    modified.end = start_time;
    database.modifyInterval(latest, modified, verbose);
    if (verbose)
    {
      std::cout << '\n' << intervalSummarize (database, rules, modified);
    }
  }

  validate (cli, rules, database, to_copy);
  database.addInterval (to_copy, verbose);

  journal.endTransaction ();

  if (verbose)
  {
    std::cout << intervalSummarize (database, rules, to_copy);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
