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
#include <Pig.h>
#include <format.h>
#include <vector>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
bool domGet (
  Database& database,
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
        value = latest.range.is_open () ? "1" : "0";
        return true;
      }

      // dom.active.start
      if (pig.skipLiteral (".start") &&
          latest.range.is_open ())
      {
        value = latest.range.start.toISOLocalExtended ();
        return true;
      }

      // dom.active.tag.count
      if (pig.skipLiteral (".tag.count"))
      {
        value = format ("{1}", latest.tags ().size ());
        return true;
      }

      // dom.active.tag.<N>
      pig.save ();
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

      pig.restore ();
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
