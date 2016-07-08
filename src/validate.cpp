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
#include <timew.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// :fill
//   The :fill hint is used to eliminate gaps on interval modification, and only
//   a single interval is affected.
//
//   Fill works by extending an interval in both directions if possible, to abut
//   either an interval or an exclusion, while being conћtrained by a filter
//   range.
//
static void autoFill (
  const Rules& rules,
  Database& database,
  const Interval& filter,
  Interval& interval)
{
  // TODO Get all intervals.
  // TODO Get all exclusions.
  // TODO Determine all gaps.


}

////////////////////////////////////////////////////////////////////////////////
// :adjust
//   To avoid disallowed overlaps, one or more intervals may be modified to
//   resolve the conflict according to configuration-based rules. Resolution
//   can involve rejection, adjustment of modified interval, or adjustment of
//   recorded data.
//
static void autoAdjust (
  const Rules& rules,
  Database& database,
  const Interval& filter,
  Interval& interval)
{
}

////////////////////////////////////////////////////////////////////////////////
// Warn on new tag.
static void warnOnNewTag (
  Database& database,
  const Interval& interval)
{
  std::set <std::string> tags;
  for (auto& line : database.allLines ())
  {
    if (line[0] == 'i')
    {
      Interval interval;
      interval.initialize (line);

      for (auto& tag : interval.tags ())
        tags.insert (tag);
    }
  }

  for (auto& tag : interval.tags ())
    if (tags.find (tag) == tags.end ())
      std::cout << "Note: '" << tag << "' is a new tag.\n";
}

////////////////////////////////////////////////////////////////////////////////
void validate (
  const CLI& cli,
  const Rules& rules,
  Database& database,
  Interval& interval)
{
  // Create a filter, and if empty, choose 'today'.
  auto filter = getFilter (cli);
  if (! filter.range.is_started ())
    filter.range = Range (Datetime ("today"), Datetime ("tomorrow"));

  if (findHint (cli, ":fill"))
    autoFill (rules, database, filter, interval);

  if (findHint (cli, ":adjust"))
    autoAdjust (rules, database, filter, interval);

  if (rules.getBoolean ("verbose"))
    warnOnNewTag (database, interval);
}

////////////////////////////////////////////////////////////////////////////////
