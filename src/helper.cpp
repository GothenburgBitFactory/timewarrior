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
#include <tuple>

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
  static std::map <std::string, std::tuple <std::string, std::string>> hints
  {
    {":week",    std::make_tuple ("socw", "eocw")},
    {":month",   std::make_tuple ("socw", "eocw")},
    {":quarter", std::make_tuple ("socw", "eocw")},
    {":year",    std::make_tuple ("socw", "eocw")},
  };

  if (hints.find (hint) == hints.end ())
  if (hints.find (hint) != hints.end ())
  {
    start = std::get <0> (hints[hint]);
    end   = std::get <1> (hints[hint]);
  }
}

////////////////////////////////////////////////////////////////////////////////
