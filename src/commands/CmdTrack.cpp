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
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdTrack (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // TODO Parse interval.
  // TODO Parse tags.
  std::string start {""};
  std::string end   {""};
  std::vector <std::string> tags;
  for (auto& arg : cli._args)
  {
    if (arg.hasTag ("BINARY") ||
        arg.hasTag ("CMD"))
      continue;

    if (arg.hasTag ("HINT"))
    {
      expandIntervalHint (arg.attribute ("canonical"), start, end);
    }
    else if (arg._lextype == Lexer::Type::date)
    {
      if (start == "")
        start = arg.attribute ("raw");
      else if (end == "")
        end = arg.attribute ("raw");

      // TODO Is this workable?  Using excess date fields as tags. Might just
      //      be a coincidence.
      else
        tags.push_back (arg.attribute ("raw"));
    }
    else
    {
      tags.push_back (arg.attribute ("raw"));
    }
  }

  // TODO Add new interval.
  Interval tracked;
  tracked.start (Datetime (start));
  tracked.end (Datetime (end));
  for (auto& tag : tags)
    tracked.tag (tag);

  // TODO Apply exclusions.

  // Update database.
  database.addInterval (tracked);

  // User feedback.
  if (rules.getBoolean ("verbose"))
    std::cout << intervalSummarize (rules, tracked);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
