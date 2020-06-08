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
int CmdStart (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  auto verbose = rules.getBoolean ("verbose");
  const Datetime now {};

  auto interval = cli.getFilter ({now, 0});

  if (interval.start > now)
  {
    throw std::string ("Time tracking cannot be set in the future.");
  }
  else if (!interval.is_started ())
  {
    // The :all hint provides a filter that is neither started nor ended, which
    // the start command cannot handle and we do not want to auto start it now.
    throw std::string ("Interval start must be specified");
  }
  else if (interval.is_ended ())
  {
    return CmdTrack (cli, rules, database, journal);
  }

  auto latest = getLatestInterval (database);
  if (latest.is_open ())
  {
    // If the new interval tags match those of the currently open interval, then
    // do nothing - the tags are already being tracked.
    if (latest.encloses (interval) && latest.tags () == interval.tags ())
    {
      if (verbose)
      {
        std::cout << intervalSummarize (database, rules, latest);
      }
      return 0;
    }
  }

  journal.startTransaction ();
  validate (cli, rules, database, interval);
  database.addInterval (interval, verbose);
  journal.endTransaction ();

  if (verbose)
  {
    std::cout << intervalSummarize (database, rules, interval);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
