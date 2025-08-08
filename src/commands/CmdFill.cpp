////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2018, 2020 - 2024, Gothenburg Bit Factory.
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

#include <IntervalFilterAllInRange.h>
#include <IntervalFilterAllWithIds.h>
#include <IntervalFilterFirstOf.h>
#include <commands.h>
#include <format.h>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
int CmdFill (
  CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  const bool verbose = rules.getBoolean ("verbose");
  const bool do_adjust = cli.getHint ("adjust", false);

  const auto ids = cli.getIds ();

  Interval interval;

  if (ids.size () > 1)
  {
    throw std::string ("Only one ID may be specified. See 'timew help fill'.");
  }

  if (ids.empty ())
  {
    IntervalFilterFirstOf filtering {std::make_shared <IntervalFilterAllInRange> (Range {})};
    auto const intervals = getTracked (database, rules, filtering);

    if (intervals.empty ())
    {
      throw std::string ("There is no active time tracking.");
    }

    interval = intervals.at (0);

    if (! interval.is_open ())
    {
    throw std::string ("ID must be specified. See 'timew help fill'.");
    }
  }
  else
  {
    auto filtering = IntervalFilterAllWithIds (ids);
    const auto intervals = getTracked (database, rules, filtering);

    if (intervals.empty ())
    {
      throw format ("ID '@{1}' does not correspond to any tracking.", *ids.begin ());
    }

    interval = intervals.at (0);
  }

  Interval modified {interval};

  fillRange (rules, database, modified);

  journal.startTransaction ();

  database.deleteInterval (interval);
  autoAdjust (do_adjust, rules, database, modified);
  database.addInterval (modified, verbose);

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
