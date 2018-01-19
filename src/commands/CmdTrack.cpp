////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Paul Beckingham, Federico Hernandez.
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

#include <commands.h>
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdTrack (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  auto filter = getFilter (cli);

  // If this is not a proper closed interval, then the user is trying to make
  // the 'track' command behave like 'start', so delegate to CmdStart.
  if (! filter.range.is_started () ||
      ! filter.range.is_ended ())
    return CmdStart (cli, rules, database);

  // Validation must occur before flattening.
  validate (cli, rules, database, filter);

  for (auto& interval : flatten (filter, getAllExclusions (rules, filter.range)))
  {
    database.addInterval (interval);

    if (rules.getBoolean ("verbose"))
      std::cout << intervalSummarize (database, rules, interval);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
