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
#include <Duration.h>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
Color tagColor (const Rules& rules, const std::string& tag)
{
  Color c;
  std::string name = std::string ("tag.") + tag + ".color";
  if (rules.has (name))
    c = Color (rules.get (name));

  return c;
}

////////////////////////////////////////////////////////////////////////////////
std::string intervalSummarize (const Rules& rules, const Interval& interval)
{
  std::stringstream out;
  if (interval.isStarted ())
  {
    // Combine and colorize tags.
    std::string tags;
    for (auto& tag : interval.tags ())
    {
      if (tags != "")
        tags += " ";

      tags += tagColor (rules, tag).colorize (quoteIfNeeded (tag));
    }

    // Interval closed.
    if (interval.isEnded ())
    {
      Duration dur (Datetime (interval.end ()) - Datetime (interval.start ()));
      out << "Recorded " << tags << "\n"
          << "  Started " << interval.start ().toISOLocalExtended () << "\n"
          << "  Ended   " << interval.end ().toISOLocalExtended () << "\n"
          << "  Elapsed " << std::setw (19) << std::setfill (' ') << dur.format () << "\n";
    }

    // Interval open.
    else
    {
      Duration dur (Datetime () - interval.start ());
      out << "Tracking " << tags << "\n"
          << "  Started " << interval.start ().toISOLocalExtended () << "\n";

      if (dur.toTime_t () > 10)
        out << "  Elapsed " << std::setw (19) << std::setfill (' ') << dur.format () << "\n";
    }
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
void expandIntervalHint (
  const std::string& hint,
  std::string& start,
  std::string& end)
{
  static std::map <std::string, std::vector <std::string>> hints
  {
    {":day",     {"today", "tomorrow"}},
    {":week",    {"socw",  "eocw"}},
    {":month",   {"socw",  "eocw"}},
    {":quarter", {"socw",  "eocw"}},
    {":year",    {"socw",  "eocw"}},
  };

  if (hints.find (hint) != hints.end ())
  {
    start = hints[hint][0];
    end   = hints[hint][1];
  }
}

////////////////////////////////////////////////////////////////////////////////
// A filter is a placeholder for a start datetime, end datetime and a set of
// tags, which makes it essentially an interval.
Filter createFilterFromCLI (const CLI& cli)
{
  Filter filter;
  std::string start;
  std::string end;
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
        filter.tag (arg.attribute ("raw"));
    }
    else
    {
      filter.tag (arg.attribute ("raw"));
    }
  }

  if (start != "") filter.start (Datetime (start));
  if (end   != "")   filter.end (Datetime (end));

  return filter;
}

////////////////////////////////////////////////////////////////////////////////
// The five different possibilities:
//
//               timeline.start      timeline.end
//               |                   |
//   [--------]  |                   |
//          [----|----]              |
//               |  [------------]   |
//               |              [----|----]
//               |                   |   [--------]
//          [----|-------------------|----]
//               |                   |
//
Timeline createTimelineFromData (
  const Rules& rules,
  Database& database,
  const Filter& filter)
{
  Timeline t;
  t.start (filter.start ());
  t.end (filter.end ());

  // TODO Add filtered exclusions.

  // Add filtered intervals.
  for (auto& interval : database.getAllIntervals ())
    if (intervalMatchesFilter (interval, filter))
      t.include (interval);

  return t;
}

////////////////////////////////////////////////////////////////////////////////
Interval getLatestInterval (
  Timeline& timeline,
  Database& database,
  const Filter& filter)
{
  Interval i;

  return i;
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter if the start/end overlaps, and all filter tags
// are found in the interval.
bool intervalMatchesFilter (const Interval& interval, const Filter& filter)
{
  if ((filter.start ().toEpoch () == 0 &&
       filter.end ().toEpoch () == 0)

      ||

      (interval.end () > filter.start () &&
       interval.start () < filter.end ()))
  {
    for (auto& tag : filter.tags ())
      if (! interval.hasTag (tag))
        return false;

    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
