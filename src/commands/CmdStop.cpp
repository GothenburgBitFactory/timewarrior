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
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <Interval.h>
#include <Lexer.h>
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
  Database& database)
{
  // Load the most recent interval.
  auto filter = getFilter (cli);
  auto latest = getLatestInterval (database);

  // Verify the interval is open.
  if (! latest.range.is_open ())
    throw std::string ("There is no active time tracking.");

  Interval modified {latest};

  // If a stop date is specified (and occupies filter.range.start) then use
  // that instead of the current time.
  if (filter.range.start.toEpoch () != 0)
    modified.range.end = filter.range.start;
  else
    modified.range.end = Datetime ();

  // Close the interval.
  database.deleteInterval (latest);
  validate (cli, rules, database, modified);
  database.addInterval (modified);

  // If tags are specified, but are not a full set of tags, remove them
  // before closing the interval.
  if (filter.tags ().size () &&
      setIntersect (filter.tags (), latest.tags ()).size () != latest.tags ().size ())
  {
    for (auto& tag : filter.tags ())
      if (modified.hasTag (tag))
        modified.untag (tag);
      else
        throw format ("The current interval does not have the '{1}' tag.", tag);
  }

  latest.range.end = modified.range.end;
  if (rules.getBoolean ("verbose"))
    std::cout << intervalSummarize (database, rules, latest);

  // Open a new interval with remaining tags, if any.
  if (filter.tags ().size () &&
      modified.tags ().size () != latest.tags ().size ())
  {
    modified.range.start = modified.range.end;
    modified.range.end = {0};
    validate (cli, rules, database, modified);
    database.addInterval (modified);
    if (rules.getBoolean ("verbose"))
      std::cout << '\n' << intervalSummarize (database, rules, modified);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
