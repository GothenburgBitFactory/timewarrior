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

#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

template <class T> T setIntersect (
  const T& left, const T& right)
{
  T intersection;
  for (auto& l : left)
    if (std::find (right.begin (), right.end (), l) != right.end ())
      intersection.insert (l);

  return intersection;
}

////////////////////////////////////////////////////////////////////////////////
int CmdStop (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  auto verbose = rules.getBoolean ("verbose");

  // Load the most recent interval.
  auto filter = cli.getFilter ();
  auto latest = getLatestInterval (database);
  std::set <int> ids = cli.getIds ();

  // Verify the interval is open.
  if (! latest.is_open ())
  {
    throw std::string ("There is no active time tracking.");
  }

  // We expect no ids
  if (! ids.empty ())
  {
    throw std::string ("The stop command does not accept ids as it works on the most recent open interval only. "
                       "Perhaps you want the modify command?.");
  }

  journal.startTransaction ();

  Interval modified {latest};

  // If a stop date is specified (and occupies filter.start) then use
  // that instead of the current time.
  if (filter.start.toEpoch () != 0)
  {
    if (modified.start >= filter.start)
    {
      throw std::string ("The end of a date range must be after the start.");
    }

    modified.end = filter.start;
  }
  else
  {
    modified.end = Datetime ();
  }

  // Close the interval.
  database.deleteInterval (latest);
  validate (cli, rules, database, modified);

  for (auto& interval : flatten (modified, getAllExclusions (rules, modified)))
  {
    database.addInterval (interval, verbose);

    if (verbose)
      std::cout << intervalSummarize (database, rules, interval);
  }

  // If tags are specified, but are not a full set of tags, remove them
  // before closing the interval.
  if (! filter.tags ().empty () &&
      setIntersect (filter.tags (), latest.tags ()).size () != latest.tags ().size ())
  {
    for (auto& tag : filter.tags ())
      if (modified.hasTag (tag))
        modified.untag (tag);
      else
        throw format ("The current interval does not have the '{1}' tag.", tag);
  }

  // Open a new interval with remaining tags, if any.
  if (! filter.tags ().empty () &&
      modified.tags ().size () != latest.tags ().size ())
  {
    modified.start = modified.end;
    modified.end = {0};
    validate (cli, rules, database, modified);
    database.addInterval (modified, verbose);
    if (verbose)
      std::cout << '\n' << intervalSummarize (database, rules, modified);
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
