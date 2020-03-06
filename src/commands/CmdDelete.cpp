////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <timew.h>
#include <format.h>
#include <iostream>
#include <src/Journal.h>

////////////////////////////////////////////////////////////////////////////////
int CmdDelete (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  auto verbose = rules.getBoolean ("verbose");

  // Gather IDs.
  std::set <int> ids = cli.getIds ();

  if (ids.empty ())
  {
    throw std::string ("IDs must be specified. See 'timew help delete'.");
  }

  journal.startTransaction ();

  flattenDatabase (database, rules);
  auto intervals = getIntervalsByIds (database, rules, ids);

  for (const auto& interval : intervals)
  {
    database.deleteInterval (interval);

    if (verbose)
    {
      std::cout << "Deleted @" << interval.id << '\n';
    }
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
