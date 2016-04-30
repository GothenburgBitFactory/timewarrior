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
#include <commands.h>
#include <timew.h>
#include <Interval.h>
#include <Lexer.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdStop (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Load the most recent interval.
  auto latest = getLatestInterval (database);

  // Verify the interval is open.
  if (  latest.range.started () &&
      ! latest.range.ended ())
  {
    // Stop it.
    Interval modified {latest};
    modified.range.end = Datetime ();
    database.modifyInterval (latest, modified);
/*
    // TODO There is no 1:N modifyInterval.
    datebase.deleteInterval (latest);

    // TODO Create and populate a Timeline.
    for (auto& fragment : splitInterval (modified, ?))
      database.addInterval (fragment);
*/

    // TODO intervalSummarїze needs to operate on a vector of similar intervals.
    if (rules.getBoolean ("verbose"))
      std::cout << intervalSummarize (rules, modified);

    // If tags were specified, and after removing those tags, there are still
    // tags remaining, then add a contiguous interval.
    auto words = cli.getWords ();
    if (words.size ())
      for (auto& word : cli.getWords ())
        latest.untag (Lexer::dequote (word));

    if (words.size () &&
        latest.tags ().size ())
    {
      // Contiguous with previous interval.
      latest.range.start = modified.range.end;
      latest.range.end   = Datetime (0);
      database.addInterval (latest);
      if (rules.getBoolean ("verbose"))
        std::cout << '\n' << intervalSummarize (rules, latest);
    }
  }
  else
  {
    if (rules.getBoolean ("verbose"))
      std::cout << "There is no active time tracking.\n";
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
