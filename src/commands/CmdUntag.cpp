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
#include <commands.h>
#include <format.h>
#include <timew.h>
#include <iostream>
#include <stdlib.h>
#include <IntervalFilterAllWithIds.h>
#include <IntervalFilterAllInRange.h>
#include <IntervalFilterFirstOf.h>

////////////////////////////////////////////////////////////////////////////////
int CmdUntag (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  const bool verbose = rules.getBoolean ("verbose");

  // Gather IDs and TAGs.
  std::set <int> ids = cli.getIds ();
  std::vector<std::string> tags = cli.getTags ();

  if (tags.empty ())
  {
    throw std::string ("At least one tag must be specified. See 'timew help untag'.");
  }

  journal.startTransaction ();

  flattenDatabase (database, rules);
  std::vector <Interval> intervals;

  if (ids.empty ())
  {
    IntervalFilterFirstOf filtering {std::make_shared <IntervalFilterAllInRange> (Range {})};
    auto latest = getTracked (database, rules, filtering);

    if (latest.empty ())
    {
      throw std::string ("There is no active time tracking.");
    }
    else if (!latest.at (0).is_open ())
    {
      throw std::string ("At least one ID must be specified. See 'timew help untag'.");
    }

    intervals = latest;
  }
  else
  {
    auto filtering = IntervalFilterAllWithIds (ids);
    intervals = getTracked (database, rules, filtering);

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
  }

  // Remove tags from intervals.
  for (const auto& interval : intervals)
  {
    Interval modified {interval};

    for (auto& tag : tags)
    {
      modified.untag (tag);
    }

    database.modifyInterval (interval, modified, verbose);

    if (verbose)
    {
      std::cout << "Removed " << joinQuotedIfNeeded (" ", tags) << " from @" << interval.id << '\n';
    }
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
