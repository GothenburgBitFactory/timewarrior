////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <iostream>
#include <timew.h>

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
  const bool verbose = rules.getBoolean ("verbose");
  const Datetime now {};

  // Load the most recent interval.
  auto latest = getLatestInterval (database);

  // Verify the interval is open.
  if (! latest.is_open ())
  {
    throw std::string ("There is no active time tracking.");
  }

  // We expect no ids
  if (! cli.getIds ().empty ())
  {
    throw std::string ("The stop command does not accept ids as it works on the most recent open interval only. "
                       "Perhaps you want the modify command?.");
  }

  auto range = cli.getRange ({now, 0});

  if (! range.is_started ())
  {
    throw std::string ("No datetime specified.");
  }
  else if (range.start <= latest.start)
  {
    throw std::string ("The end of a date range must be after the start.");
  }

  auto tags = cli.getTags ();

  std::set <std::string> diff = {};

  if(! std::includes(latest.tags ().begin (), latest.tags ().end (),
                     tags.begin (), tags.end ()))
  {
    std::set_difference(tags.begin (), tags.end (),
                        latest.tags ().begin (), latest.tags ().end (),
                        std::inserter(diff, diff.begin ()));

    throw format ("The current interval does not have the '{1}' tag.", *diff.begin ());
  }
  else if (! tags.empty ())
  {
    std::set_difference(latest.tags ().begin (), latest.tags ().end (),
                        tags.begin (), tags.end (),
                        std::inserter(diff, diff.begin()));
  }

  journal.startTransaction ();

  if (diff.empty ())
  {
    Interval modified {latest};
    modified.end = range.start;

    database.deleteInterval (latest);
    validate (cli, rules, database, modified);

    for (auto& interval : flatten (modified, getAllExclusions (rules, modified)))
    {
      database.addInterval (interval, verbose);

      if (verbose)
      {
        std::cout << intervalSummarize (rules, interval);
      }
    }
  }
  else
  {
    Interval next {range.start, 0};
    for (auto& tag : diff)
    {
      next.tag (tag);
    }

    validate (cli, rules, database, next);
    database.addInterval (next, verbose);

    if (verbose)
    {
      std::cout << '\n' << intervalSummarize (rules, next);
    }
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
