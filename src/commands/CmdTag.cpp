////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

////////////////////////////////////////////////////////////////////////////////
int CmdTag (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  // Gather IDs and TAGs.
  std::set <int> ids = cli.getIds ();
  std::vector<std::string> tags = cli.getTags ();

  if (tags.empty ())
  {
    throw std::string ("At least one tag must be specified. See 'timew help tag'.");
  }

  // Load the data.
  // Note: There is no filter.
  Interval filter;
  auto tracked = getTracked (database, rules, filter);

  bool dirty = true;

  journal.startTransaction ();

  for (auto& id : ids)
  {
    if (id > static_cast <int> (tracked.size ()))
      throw format ("ID '@{1}' does not correspond to any tracking.", id);

    if (tracked[tracked.size() - id].synthetic && dirty)
    {
      auto latest = getLatestInterval(database);
      auto exclusions = getAllExclusions (rules, filter);

      Interval modified {latest};

      // Update database.
      database.deleteInterval (latest);
      for (auto& interval : flatten (modified, exclusions))
        database.addInterval (interval, rules.getBoolean ("verbose"));

      dirty = false;
    }
  }

  if (ids.empty ())
  {
    if (tracked.empty ())
    {
      throw std::string ("There is no active time tracking.");
    }

    if (!tracked.back ().is_open ())
    {
      throw std::string ("At least one ID must be specified. See 'timew help tag'.");
    }

    ids.insert (1);
  }

  // Apply tags to ids.
  for (auto& id : ids)
  {
    if (id > static_cast <int> (tracked.size ()))
      throw format ("ID '@{1}' does not correspond to any tracking.", id);

    Interval i = tracked[tracked.size () - id];

    for (auto& tag : tags)
      i.tag (tag);

    //TODO validate (cli, rules, database, i);
    database.modifyInterval (tracked[tracked.size () - id], i, rules.getBoolean ("verbose"));

    if (rules.getBoolean ("verbose"))
    {
      std::cout << "Added " << joinQuotedIfNeeded (" ", tags) << " to @" << id << '\n';
    }
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
