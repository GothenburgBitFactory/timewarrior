////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <Duration.h>
#include <IntervalFilterAllInRange.h>
#include <IntervalFilterAllWithTags.h>
#include <IntervalFilterAndGroup.h>
#include <IntervalFilterFirstOf.h>
#include <Pig.h>
#include <format.h>
#include <iostream>
#include <timew.h>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Helper function to show tag count
static bool showTagCount (std::string& value, const std::set<std::string>& tags)
{
  value = format ("{1}", tags.size ());
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Helper function to show tag by index
static bool showTagByIndex (std::string& value, const std::set<std::string>& tags, const int index)
{
  if (1 <= index && index <= static_cast <int> (tags.size ()))
  {
    auto it = tags.begin ();
    std::advance (it, index - 1);
    value = format ("{1}", *it);
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Helper function to show all tags
static bool showAllTags (std::string& value, const std::set<std::string>& tags)
{
  std::stringstream s;
  s << joinQuotedIfNeeded ( " ", tags );
  value = s.str ();
  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool domGet (
  Database& database,
  Interval& filter,
  const Rules& rules,
  const std::string& reference,
  std::string& value)
{
  Pig pig (reference);
  if (pig.skipLiteral ("dom."))
  {
    // dom.active[.<...>]
    if (pig.skipLiteral ("active"))
    {
      IntervalFilterFirstOf filtering {std::make_shared <IntervalFilterAllInRange> (Range {})};
      auto intervals = getTracked (database, rules, filtering);

      // dom.active
      if (pig.eos ())
      {
        value = !intervals.empty () && intervals.at (0).is_open () ? "1" : "0";
        return true;
      }

      if (intervals.empty ())
      {
        return false;
      }

      const auto& latest = intervals.at (0);

      if (!latest.is_open())
      {
        return false;
      }

      // dom.active.start
      if (pig.skipLiteral (".start"))
      {
        value = latest.start.toISOLocalExtended ();
        return true;
      }

      // dom.active.duration
      if (pig.skipLiteral (".duration"))
      {
        value = Duration (latest.total ()).formatISO ();
        return true;
      }

      // dom.active.json
      if (pig.skipLiteral (".json"))
      {
        value = latest.json ();
        return true;
      }

      // dom.active.tag.<...>
      if (pig.skipLiteral (".tag."))
      {
        warn ("DOM reference '.tag.' is deprecated and will be removed in a future version of Timewarrior!\nUse reference '.tags.' instead.");

        // dom.active.tag.count
        if (pig.skipLiteral ("count"))
        {
          return showTagCount (value, latest.tags ());
        }

        // dom.active.tag.<N>
        if (int n; pig.getDigits (n))
        {
          return showTagByIndex (value, latest.tags (), n);
        }
      }

      // dom.active.tags[.<...>]
      if (pig.skipLiteral (".tags"))
      {
        // dom.active.tags
        if (pig.eos ()) {
          return showAllTags (value, latest.tags());
        }

        // dom.active.tags.<...>
        if (pig.skipLiteral ("."))
        {
          // dom.active.tags.count
          if (pig.skipLiteral ("count"))
          {
            return showTagCount (value, latest.tags ());
          }

          // dom.active.tags.<N>
          if (int n; pig.getDigits (n))
          {
            return showTagByIndex (value, latest.tags (), n);
          }
        }
      }
    }

    // dom.tracked.<...>
    else if (pig.skipLiteral ("tracked."))
    {
      IntervalFilterAndGroup filtering ({
        std::make_shared <IntervalFilterAllInRange> (Range {filter.start, filter.end}),
        std::make_shared <IntervalFilterAllWithTags> (filter.tags ())
      });

      auto tracked = getTracked (database, rules, filtering);
      int count = static_cast <int> (tracked.size ());

      // dom.tracked.tags[.<...>]
      if (pig.skipLiteral ("tags"))
      {
        std::set <std::string> tags;
        for (const auto& interval : tracked)
        {
          for (const auto& tag : interval.tags ())
          {
            tags.insert (tag);
          }
        }

        // dom.tracked.tags
        if (pig.eos ())
        {
          return showAllTags (value, tags);
        }

        // dom.tracked.tags.<...>
        if (pig.skipLiteral ("."))
        {
          // dom.tracked.tags.count
          if (pig.skipLiteral ("count"))
          {
            return showTagCount (value, tags);
          }

          // dom.tracked.tags.<M>
          if (int m; pig.getDigits (m))
          {
            return showTagByIndex (value, tags, m);
          }
        }
      }

      // dom.tracked.ids
      if (pig.skipLiteral ("ids"))
      {
        std::stringstream s;
        for (auto& interval : tracked)
        {
          s << format ( "@{1} ", interval.id );
        }
        value = s.str ();
        return true;
      }

      // dom.tracked.count
      if (pig.skipLiteral ("count"))
      {
        value = format ("{1}", tracked.size ());
        return true;
      }

      // dom.tracked.<N>.<...>
      if (int n; pig.getDigits (n) &&
          n <= count        &&
          pig.skipLiteral ("."))
      {
        // dom.tracked.<N>.start
        if (pig.skipLiteral ("start"))
        {
          value = tracked[count - n].start.toISOLocalExtended ();
          return true;
        }

        // dom.tracked.<N>.end
        if (pig.skipLiteral ("end"))
        {
          if (tracked[count -n].is_open ())
            value = "";
          else
            value = tracked[count - n].end.toISOLocalExtended ();
          return true;
        }

        // dom.tracked.<N>.duration
        if (pig.skipLiteral ("duration"))
        {
          value = Duration (tracked[count - n].total ()).formatISO ();
          return true;
        }

        // dom.tracked.<N>.json
        if (pig.skipLiteral ("json"))
        {
          value = tracked[count - n].json ();
          return true;
        }

        // dom.tracked.<N>.tag.<...>
        if (pig.skipLiteral ("tag."))
        {
          warn ("DOM reference '.tag.' is deprecated and will be removed in a future version of Timewarrior!\nUse reference '.tags.' instead.");

          // dom.tracked.<N>.tag.count
          if (pig.skipLiteral ("count"))
          {
            return showTagCount (value, tracked[count - n].tags ());
          }

          // dom.tracked.<N>.tag.<M>
          if (int m; pig.getDigits (m))
          {
            return showTagByIndex (value, tracked[count - n].tags (), m);
          }
        }

        // dom.tracked.<N>.tags[.<...>]
        if (pig.skipLiteral ("tags"))
        {
          // dom.tracked.<N>.tags
          if (pig.eos ())
          {
            return showAllTags (value, tracked[count - n].tags ());
          }

          // dom.tracked.<N>.tags.<...>
          if (pig.skipLiteral ("."))
          {
            // dom.tracked.<N>.tags.count
            if (pig.skipLiteral ("count"))
            {
              return showTagCount (value, tracked[count - n].tags ());
            }

            // dom.tracked.<N>.tags.<M>
            if (int m; pig.getDigits (m))
            {
              return showTagByIndex (value, tracked[count - n].tags (), m);
            }
          }
        }
      }
    }

    // dom.tag.<...>
    else if (pig.skipLiteral ("tag."))
    {
      warn ( "DOM reference '.tag.' is deprecated and will be removed in a future version of Timewarrior!\nUse reference '.tags.' instead.");

      // get unique, ordered list of tags.
      std::set <std::string> tags = database.tags ();

      // dom.tag.count
      if (pig.skipLiteral ("count"))
      {
        return showTagCount (value, tags);
      }

      // dom.tag.<N>
      if (int n; pig.getDigits (n))
      {
        return showTagByIndex (value, tags, n);
      }
    }

    // dom.tags[.<...>]
    else if (pig.skipLiteral ("tags"))
    {
      // get unique, ordered list of tags.
      std::set <std::string> tags = database.tags ();

      // dom.tags
      if (pig.eos ())
      {
        return showAllTags (value, tags);
      }

      // dom.tags.count
      if (pig.skipLiteral (".count"))
      {
        return showTagCount (value, tags);
      }

      // dom.tags.<N>
      if (int n; pig.skipLiteral(".") && pig.getDigits (n))
      {
        return showTagByIndex (value, tags, n);
      }
    }

    // dom.rc.<name>
    else if (pig.skipLiteral ("rc."))
    {
      std::string name;
      if (pig.getRemainder (name))
      {
        value = rules.get (name);
        return true;
      }
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
