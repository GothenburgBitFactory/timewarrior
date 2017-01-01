////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2017, Paul Beckingham, Federico Hernandez.
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
#include <Datetime.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
int CmdMove (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Gather ID and TAGs.
  int id = 0;
  std::string new_start;
  for (auto& arg : cli._args)
  {
    if (arg.hasTag ("ID"))
    {
      if (id)
        throw std::string ("The 'move' command only supports a single ID.");
      else
        id = strtol (arg.attribute ("value").c_str (), NULL, 10);
    }

    if (arg.hasTag ("FILTER") &&
        arg._lextype == Lexer::Type::date)
      new_start = arg.attribute ("raw");
  }

  if (! id)
    throw std::string ("ID must be specified. See 'timew help move'.");

  // Load the data.
  // Note: There is no filter.
  Interval filter;
  auto tracked = getTracked (database, rules, filter);

  if (id > static_cast <int> (tracked.size ()))
    throw format ("ID '@{1}' does not correspond to any tracking.", id);

  // Move start time.
  Interval i = tracked[tracked.size () - id];
  Datetime start (new_start);

  // Changing the start date should also change the end date by the same
  // amount.
  if (i.range.start < start)
  {
    auto delta = start - i.range.start;
    i.range.start = start;
    if (! i.range.is_open ())
      i.range.end += delta;
  }
  else
  {
    auto delta = i.range.start - start;
    i.range.start = start;
    if (! i.range.is_open ())
      i.range.end -= delta;
  }

  database.deleteInterval (tracked[tracked.size () - id]);

  validate (cli, rules, database, i);
  database.addInterval (i);

  if (rules.getBoolean ("verbose"))
    std::cout << "Moved @" << id << " to " << i.range.start.toISOLocalExtended () << '\n';

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
