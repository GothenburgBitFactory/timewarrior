////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdTrack (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  auto verbose = rules.getBoolean ("verbose");

  auto filter = cli.getFilter ();

  // If this is not a proper closed interval, then the user is trying to make
  // the 'track' command behave like 'start', so delegate to CmdStart.
  if (! filter.is_started () ||
      ! filter.is_ended ())
    return CmdStart (cli, rules, database, journal);

  journal.startTransaction ();

  // Validation must occur before flattening.
  validate (cli, rules, database, filter);

  for (auto& interval : flatten (filter, getAllExclusions (rules, filter)))
  {
    database.addInterval (interval, verbose);

    if (verbose)
    {
      timew::cout << intervalSummarize (database, rules, interval);
    }
  }

  journal.endTransaction ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
