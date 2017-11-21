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
#include <Duration.h>
#include <format.h>
#include <commands.h>
#include <timew.h>
#include <iostream>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
int CmdSplit (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  std::vector <int> ids = cli.getIds();

  if (ids.empty ())
    throw std::string ("IDs must be specified. See 'timew help split'.");

  // Load the data.
  // Note: There is no filter.
  Interval filter;
  auto tracked = getTracked (database, rules, filter);

  // Apply tags to ids.
  for (auto& id : ids)
  {
    if (id > static_cast <int> (tracked.size ()))
      throw format ("ID '@{1}' does not correspond to any tracking.", id);

    Interval first = tracked[tracked.size () - id];
    Interval second = first;

    if (first.range.is_open ())
    {
      Datetime midpoint;
      midpoint -= (midpoint - first.range.start) / 2;
      first.range.end = midpoint;
      second.range.start = midpoint;
    }
    else
    {
      Datetime midpoint = first.range.start;
      midpoint += (first.range.end - first.range.start) / 2;
      first.range.end = midpoint;
      second.range.start = midpoint;
    }

    database.deleteInterval (tracked[tracked.size () - id]);

    validate (cli, rules, database, first);
    database.addInterval (first);

    validate (cli, rules, database, second);
    database.addInterval (second);

    if (rules.getBoolean ("verbose"))
      std::cout << "Split @" << id << '\n';
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
