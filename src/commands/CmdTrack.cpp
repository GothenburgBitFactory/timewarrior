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

#include <commands.h>
#include <iostream>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
int CmdTrack (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  const bool verbose = rules.getBoolean ("verbose");

  // We expect no ids
  if (! cli.getIds ().empty ())
  {
    throw std::string ("The track command does not accept ids. "
                       "Perhaps you want the continue command?");
  }

  auto range = cli.getRange ();

  // If this is not a proper closed interval, then the user is trying to make
  // the 'track' command behave like 'start', so delegate to CmdStart.
  if (! range.is_started () || ! range.is_ended ())
  {
    return CmdStart (cli, rules, database, journal);
  }

  auto tags = cli.getTags ();

  journal.startTransaction ();

  auto filter = Interval {range, tags};

  // Validation must occur before flattening.
  validate (cli, rules, database, filter);

  for (auto& interval : flatten (filter, getAllExclusions (rules, range)))
  {
    database.addInterval (interval, verbose);

    if (verbose)
    {
      std::cout << intervalSummarize (rules, interval);
    }
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
