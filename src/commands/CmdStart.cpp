////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
int CmdStart (
  const CLI& cli,
  Rules& rules,
  Database& database,
  Journal& journal)
{
  const bool verbose = rules.getBoolean ("verbose");
  const Datetime now {};

  auto range = cli.getRange ({now, 0});
  auto tags = cli.getTags ();

  if (range.start > now)
  {
    throw std::string ("Time tracking cannot be set in the future.");
  }
  else if (! range.is_started () || range.is_ended ())
  {
    throw std::string ("The start command does not accept ranges but only a single datetime. "
                       "Perhaps you want the track command?");
  }

  // We expect no ids
  if (! cli.getIds ().empty ())
  {
    throw std::string ("The start command does not accept ids. "
                       "Perhaps you want the continue command?");
  }

  journal.startTransaction ();
  auto interval = Interval {range, tags};

  if (validate (cli, rules, database, interval))
  {
    database.addInterval (interval, verbose);
    journal.endTransaction ();
  }
  if (verbose)
  {
    std::cout << intervalSummarize (rules, interval);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
