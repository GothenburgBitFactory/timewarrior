////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <cassert>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdModify (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  auto filter = getFilter (cli);
  std::set <int> ids = cli.getIds ();
  std::vector <std::string> words = cli.getWords ();
  enum { MODIFY_START, MODIFY_END } op = MODIFY_START;
  bool verbose = rules.getBoolean ("verbose");

  if (words.empty())
  {
    throw std::string ("Must specify start|end command to modify. See 'timew help modify'.");
  }

  if (words.at (0) == "start")
  {
    op = MODIFY_START;
  }
  else if (words.at (0) == "end")
  {
    op = MODIFY_END;
  }
  else
  {
    throw format ("Must specify start|end command to modify. See 'timew help modify'.", words.at (0));
  }

  if (ids.empty ())
  {
    throw std::string ("ID must be specified. See 'timew help modify'.");
  }

  if (ids.size () > 1)
  {
    throw std::string ("Only one ID may be specified. See 'timew help modify'.");
  }

  int id = *ids.begin();

  flattenDatabase (database, rules);
  auto intervals = getIntervalsByIds (database, rules, ids);
  if (intervals.size () == 0)
  {
    throw format ("ID '@{1}' does not correspond to any tracking.", id);
  }

  assert (intervals.size () == 1);
  if (filter.start.toEpoch () == 0)
  {
    throw std::string ("No updated time specified. See 'timew help modify'.");
  }

  const Interval interval = intervals.at (0);
  Interval modified {interval};
  switch (op)
  {
  case MODIFY_START:
    modified.start = filter.start;
    break;

  case MODIFY_END:
    if (interval.is_open ())
    {
      throw format ("Cannot modify end of open interval @{1}.", id);
    }
    modified.end = filter.start;
    break;
  }

  if (!modified.is_open () && (modified.start > modified.end))
  {
    throw format ("Cannot modify interval @{1} where start is after end.", id);
  }
  
  journal.startTransaction ();

  database.deleteInterval (interval);
  validate(cli, rules, database, modified);
  database.addInterval(modified, verbose);

  journal.endTransaction();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
