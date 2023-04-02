////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2022, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <Duration.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>
#include <stdlib.h>
#include <IntervalFilterAllWithIds.h>

////////////////////////////////////////////////////////////////////////////////
int CmdShorten (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  const bool verbose = rules.getBoolean ("verbose");

  std::set <int> ids = cli.getIds ();

  if (ids.empty ())
  {
    throw std::string ("IDs must be specified. See 'timew help shorten'.");
  }

  Duration dur = cli.getDuration ();

  journal.startTransaction ();

  flattenDatabase (database, rules);
  auto filtering = IntervalFilterAllWithIds (ids);
  auto intervals = getTracked (database, rules, filtering);

  if (intervals.size () != ids.size ())
  {
    for (auto& id: ids)
    {
      bool found = false;

      for (auto& interval: intervals)
      {
        if (interval.id == id)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        throw format ("ID '@{1}' does not correspond to any tracking.", id);
      }
    }
  }

  // Shorten intervals specified by ids
  for (auto& interval : intervals)
  {
    if (interval.is_open ())
    {
      throw format ("Cannot shorten open interval @{1}", interval.id);
    }

    if (dur > (interval.end - interval.start))
    {
      throw format ("Cannot shorten interval @{1} by {2} because it is only {3} in length.",
                    interval.id, dur.formatHours (),
                    Duration (interval.end - interval.start).formatHours ());
    }

    database.deleteInterval (interval);

    interval.end -= dur.toTime_t ();
    validate (cli, rules, database, interval);
    database.addInterval (interval, verbose);

    if (verbose)
    {
      std::cout << "Shortened @" << interval.id << " by " << dur.formatHours () << '\n';
    }
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
