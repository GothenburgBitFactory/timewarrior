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
#include <shared.h>
#include <format.h>
#include <Datetime.h>
#include <Duration.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Select a color to represent the interval.
Color intervalColor (
  const Interval& interval,
  const Rules& rules,
  std::map <std::string, Color>& tag_colors)
{
  Color c;
  std::string first_tag;
  for (auto& tag : interval.tags ())
  {
    if (first_tag == "")
      first_tag = tag;

    std::string name = std::string ("tags.") + tag + ".color";
    if (rules.has (name))
      c.blend (Color (rules.get (name)));
  }

  if (c.nontrivial ())
    return c;

  if (interval.tags ().size ())
    return tag_colors[first_tag];

  return c;
}

////////////////////////////////////////////////////////////////////////////////
// Consult rules to find any defined color for the given tag, and colorize it.
Color tagColor (const Rules& rules, const std::string& tag)
{
  Color c;
  std::string name = std::string ("tags.") + tag + ".color";
  if (rules.has (name))
    c = Color (rules.get (name));

  return c;
}

////////////////////////////////////////////////////////////////////////////////
// Summarize either an active or closed interval, for user feedback.
std::string intervalSummarize (
  Database& database,
  const Rules& rules,
  const Interval& interval)
{
  std::stringstream out;

  if (interval.range.is_started ())
  {
    // Walk backwards through the inclusions, and stop as soon as the tags
    // no longer match interval. This means the 'total' is the sum of all time
    // in the most recent set of intervals for the same tags. This is the
    // acceptable definition of "the current task".
    time_t total_recorded = 0;
    auto inclusions = getAllInclusions (database);
    std::vector <Interval>::reverse_iterator i;
    for (i = inclusions.rbegin (); i != inclusions.rend (); i++)
      if (interval.tags () == i->tags ())
        total_recorded += i->range.total ();
      else
        break;

    Duration total (total_recorded);

    // Combine and colorize tags.
    std::string tags;
    for (auto& tag : interval.tags ())
    {
      if (tags != "")
        tags += " ";

      tags += tagColor (rules, tag).colorize (quoteIfNeeded (tag));
    }

    // Interval open.
    if (interval.range.is_open ())
    {
      out << "Tracking " << tags << '\n'
          << "  Started " << interval.range.start.toISOLocalExtended () << '\n'
          << "  Current " << minimalDelta (interval.range.start, Datetime ()) << '\n'
          << "  Total   " << std::setw (19) << std::setfill (' ') << total.formatHours () << '\n';
    }

    // Interval closed.
    else
    {
      out << "Recorded " << tags << '\n'
          << "  Started " << interval.range.start.toISOLocalExtended () << '\n'
          << "  Ended   " << minimalDelta (interval.range.start, interval.range.end) << '\n'
          << "  Total   " << std::setw (19) << std::setfill (' ') << total.formatHours () << '\n';
    }
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Convert a set of hints to equivalent date ranges.
bool expandIntervalHint (
  const std::string& hint,
  Range& range)
{
  static std::map <std::string, std::vector <std::string>> hints
  {
    {":yesterday",   {"yesterday", "today"}},
    {":day",         {"today",     "eod"}},
    {":week",        {"sow",       "eow"}},
    {":month",       {"som",       "eom"}},
    {":quarter",     {"soq",       "eoq"}},
    {":year",        {"soy",       "eoy"}},
  };

  static std::vector <std::string> dayNames
  {
    ":sunday",
    ":monday",
    ":tuesday",
    ":wednesday",
    ":thursday",
    ":friday",
    ":saturday"
  };

  // Some hints are just synonyms.
  if (hints.find (hint) != hints.end ())
  {
    range.start = Datetime (hints[hint][0]);
    range.end   = Datetime (hints[hint][1]);
    debug (format ("Hint {1} expanded to {2} - {3}",
                   hint,
                   range.start.toISOLocalExtended (),
                   range.end.toISOLocalExtended ()));
    return true;
  }

  // Some require math.
  if (hint == ":lastweek")
  {
    // Note: Simply subtracting (7 * 86400) from sow, eow fails to consider
    //       daylight savings.
    Datetime sow ("sow");
    int sy = sow.year ();
    int sm = sow.month ();
    int sd = sow.day ();

    Datetime eow ("eow");
    int ey = eow.year ();
    int em = eow.month ();
    int ed = eow.day ();

    sd -= 7;
    if (sd < 1)
    {
      --sm;
      if (sm < 1)
      {
        --sy;
        sm = 12;
      }

      sd += Datetime::daysInMonth (sy, sm);
    }

    ed -= 7;
    if (ed < 1)
    {
      --em;
      if (em < 1)
      {
        --ey;
        em = 12;
      }

      ed += Datetime::daysInMonth (ey, em);
    }

    range.start = Datetime (sy, sm, sd);
    range.end   = Datetime (ey, em, ed);
    debug (format ("Hint {1} expanded to {2} - {3}",
                   hint,
                   range.start.toISOLocalExtended (),
                   range.end.toISOLocalExtended ()));
    return true;
  }
  else if (hint == ":lastmonth")
  {
    Datetime now;
    int y = now.year ();
    int y_prev = y;

    int m = now.month ();
    int m_prev = m - 1;

    if (m_prev == 0)
    {
      m_prev = 12;
      --y_prev;
    }

    range.start = Datetime (y_prev, m_prev, 1);
    range.end   = Datetime (y,      m,      1);
    debug (format ("Hint {1} expanded to {2} - {3}",
                   hint,
                   range.start.toISOLocalExtended (),
                   range.end.toISOLocalExtended ()));
    return true;
  }
  else if (hint == ":lastquarter")
  {
    Datetime now;
    int y = now.year ();
    int m = now.month ();
    int q = ((m - 1) / 3) + 1;

    if (--q == 0)
    {
      q = 4;
      --y;
    }

    m = ((q - 1) * 3) + 1;

    range.start = Datetime (y, m,     1);
    range.end   = Datetime (y, m + 3, 1);

    debug (format ("Hint {1} expanded to {2} - {3}",
                   hint,
                   range.start.toISOLocalExtended (),
                   range.end.toISOLocalExtended ()));
    return true;
  }
  else if (hint == ":lastyear")
  {
    Datetime now;
    range.start = Datetime (now.year () - 1,  1,  1);
    range.end   = Datetime (now.year (),      1,  1);
    debug (format ("Hint {1} expanded to {2} - {3}",
                   hint,
                   range.start.toISOLocalExtended (),
                   range.end.toISOLocalExtended ()));
    return true;
  }
  else if (std::find (dayNames.begin (), dayNames.end (), hint) != dayNames.end ())
  {
    int wd = std::find (dayNames.begin (), dayNames.end (), hint) - dayNames.begin ();

    Datetime now;
    int dow = now.dayOfWeek ();
    Datetime sd = now - (86400 * dow) + (86400 * (wd - 7 * (wd <= dow ? 0 : 1)));
    Datetime ed = sd + 86400;

    range.start = Datetime (sd.year(), sd.month(), sd.day());
    range.end   = Datetime (ed.year(), ed.month(), ed.day());

    debug (format ("Hint {1} expanded to {2} - {3}",
                   hint,
                   range.start.toISOLocalExtended (),
                   range.end.toISOLocalExtended ()));
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Compose a JSON document of intervals. In the trivial case:
//   [
//   ]
//
// In the non-trivial case:
//   [
//   {...},
//   {...}
//   ]
//
std::string jsonFromIntervals (const std::vector <Interval>& intervals)
{
  std::stringstream out;

  out << "[\n";
  int counter = 0;
  for (auto& interval : intervals)
  {
    if (counter)
      out << ",\n";

    out << interval.json ();
    ++counter;
  }

  if (counter)
    out << '\n';

  out << "]\n";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
Palette createPalette (const Rules& rules)
{
  Palette p;
  auto colors = rules.all ("theme.palette.color");
  if (colors.size ())
  {
    p.clear ();
    for (auto& c : colors)
      p.add (Color (rules.get (c)));
  }

  p.enabled = rules.getBoolean ("color");
  return p;
}

////////////////////////////////////////////////////////////////////////////////
// Extract the tags from a set of intervals, and using a rotating color palette,
// map unique tags to color.
//
// If there is a tags.<tag>.color setting, use it.
std::map <std::string, Color> createTagColorMap (
  const Rules& rules,
  Palette& palette,
  const std::vector <Interval>& intervals)
{
  std::map <std::string, Color> mapping;
  for (auto& interval : intervals)
  {
    for (auto& tag : interval.tags ())
    {
      std::string custom = "tags." + tag + ".color";
      if (rules.has (custom))
        mapping[tag] = Color (rules.get (custom));
      else if (mapping.find (tag) == mapping.end ())
        mapping[tag] = palette.next ();
    }
  }

  return mapping;
}

////////////////////////////////////////////////////////////////////////////////
int quantizeToNMinutes (const int minutes, const int N)
{
  if (minutes % N == 0)
    return minutes;

  auto deviation = minutes % N;
  if (deviation < N/2)
    return minutes - deviation;

  return minutes + N - deviation;
}

////////////////////////////////////////////////////////////////////////////////
// Check rules to see if day is a holiday.
bool dayIsHoliday (const Rules& rules, const Datetime& day)
{
  for (auto& holiday : rules.all ("holidays."))
    if (day.sameDay (Datetime (holiday.substr (holiday.length () - 10), "Y_M_D")))
      return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool findHint (const CLI& cli, const std::string& hint)
{
  for (auto& arg : cli._args)
    if (arg.hasTag ("HINT") &&
        arg.getToken () == hint)
      return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
std::string minimalDelta (const Datetime& left, const Datetime& right)
{
  std::string result = right.toISOLocalExtended ();

  if (left.sameYear (right))
  {
    result.replace (0, 5, "     ");
    if (left.sameMonth (right))
    {
      result.replace (5, 3, "   ");
      if (left.sameDay (right))
      {
        result.replace (8, 3, "   ");
        if (left.sameHour (right))
        {
          result.replace (11, 3, "   ");
          if (left.minute () == right.minute ())
            result.replace (14, 3, "   ");
        }
      }
    }
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
