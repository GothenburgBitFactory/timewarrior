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
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <timew.h>
#include <Pig.h>
#include <Duration.h>
#include <format.h>
#include <vector>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
bool domGet (
  Database& database,
  const Rules& rules,
  const std::string& reference,
  std::string& value)
{
  Pig pig (reference);
  if (pig.skipLiteral ("dom."))
  {
    if (pig.skipLiteral ("active"))
    {
      auto latest = getLatestInterval (database);

      // dom.active
      if (pig.eos ())
      {
        value = latest.is_open () ? "1" : "0";
        return true;
      }

      // dom.active.start
      if (pig.skipLiteral (".start") &&
          latest.is_open ())
      {
        value = latest.start.toISOLocalExtended ();
        return true;
      }

      // dom.active.duration
      if (pig.skipLiteral (".duration") &&
          latest.is_open ())
      {
        value = Duration (latest.total ()).formatISO ();
        return true;
      }

      // dom.active.tag.count
      if (pig.skipLiteral (".tag.count") &&
          latest.is_open ())
      {
        value = format ("{1}", latest.tags ().size ());
        return true;
      }

      // dom.active.json
      if (pig.skipLiteral (".json") &&
          latest.is_open ())
      {
        value = latest.json ();
        return true;
      }

      // dom.active.tag.<N>
      int n;
      if (pig.skipLiteral (".tag.") &&
          pig.getDigits (n))
      {
        if (n <= static_cast <int> (latest.tags ().size ()))
        {
          std::vector <std::string> tags;
          for (auto& tag : latest.tags ())
            tags.push_back (tag);

          value = format ("{1}", tags[n - 1]);
          return true;
        }
      }
    }

    else if (pig.skipLiteral ("tracked."))
    {
      Interval filter;
      auto tracked = getTracked (database, rules, filter);
      int count = static_cast <int> (tracked.size ());

      // dom.tracked.count
      if (pig.skipLiteral ("count"))
      {
        value = format ("{1}", tracked.size ());
        return true;
      }

      int n;
      if (pig.getDigits (n) &&
          n <= count        &&
          pig.skipLiteral ("."))
      {
        // dom.tracked.N.tag.count
        if (pig.skipLiteral ("tag.count"))
        {
          value = format ("{1}", tracked[count - n].tags ().size ());
          return true;
        }

        // dom.tracked.N.start
        if (pig.skipLiteral ("start"))
        {
          value = tracked[count - n].start.toISOLocalExtended ();
          return true;
        }

        // dom.tracked.N.end
        if (pig.skipLiteral ("end"))
        {
          if (tracked[count -n].is_open ())
            value = "";
          else
            value = tracked[count - n].end.toISOLocalExtended ();
          return true;
        }

        // dom.tracked.N.duration
        if (pig.skipLiteral ("duration"))
        {
          value = Duration (tracked[count - n].total ()).formatISO ();
          return true;
        }

        // dom.tracked.N.json
        if (pig.skipLiteral ("json"))
        {
          value = tracked[count - n].json ();
          return true;
        }

        int n;
        if (pig.skipLiteral ("tag.") &&
            pig.getDigits (n))
        {
          std::vector <std::string> tags;
          for (auto& tag : tracked[count - n].tags ())
            tags.push_back (tag);

          if (n <= static_cast <int> (tags.size ()))
          {
            value = format ("{1}", tags[n - 1]);
            return true;
          }
        }
      }
    }

    else if (pig.skipLiteral ("tag."))
    {
      // Generate a unique, ordered list of tags.
      std::set <std::string> tags;
      for (auto& interval : getAllInclusions (database))
        for (auto& tag : interval.tags ())
          tags.insert (tag);

      // dom.tag.count
      if (pig.skipLiteral ("count"))
      {
        value = format ("{1}", tags.size ());
        return true;
      }

      // dom.tag.<N>
      int n;
      if (pig.getDigits (n))
      {
        if (n <= static_cast <int> (tags.size ()))
        {
          std::vector <std::string> all;
          for (auto& tag : tags)
            all.push_back (tag);

          value = format ("{1}", all[n - 1]);
          return true;
        }
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
