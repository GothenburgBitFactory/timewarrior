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
#include <Exclusion.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (28);

  try
  {
    // exc holidays en-US
    Exclusion e;
    e.initialize ("exc holidays en-US");
    auto tokens = e.tokens ();
    t.ok (tokens.size () == 3,              "Exclusion 'exc holidays en-US' --> 3");
    t.is (tokens[0], "exc",                 "Exclusion 'exc holidays en-US' [0] --> 'exc'");
    t.is (tokens[1], "holidays",            "Exclusion 'exc holidays en-US' [1] --> 'holidays'");
    t.is (tokens[2], "en-US",               "Exclusion 'exc holidays en-US' [2] --> 'en-US'");

    // exc work 2015-11-26
    e.initialize ("exc work 2015-11-26");
    tokens = e.tokens ();
    t.ok (tokens.size () == 3,              "Exclusion 'exc work 2015-11-26' --> 3");
    t.is (tokens[0], "exc",                 "Exclusion 'exc work 2015-11-26' [0] --> 'exc'");
    t.is (tokens[1], "work",                "Exclusion 'exc work 2015-11-26' [1] --> 'work'");
    t.is (tokens[2], "2015-11-26",          "Exclusion 'exc work 2015-11-26' [2] --> '2015-11-26'");

    // exc week mon,tue,wed,thu,fri
    e.initialize ("exc week mon,tue,wed,thu,fri");
    tokens = e.tokens ();
    t.ok (tokens.size () == 3,              "Exclusion 'exc week mon,tue,wed,thu,fri' --> 3");
    t.is (tokens[0], "exc",                 "Exclusion 'exc week mon,tue,wed,thu,fri' [0] --> 'exc'");
    t.is (tokens[1], "week",                "Exclusion 'exc week mon,tue,wed,thu,fri' [1] --> 'week'");
    t.is (tokens[2], "mon,tue,wed,thu,fri", "Exclusion 'exc week mon,tue,wed,thu,fri' [2] --> 'mon,tue,wed,thu,fri'");

    // exc day start 8:30am
    e.initialize ("exc day start 8:30am");
    tokens = e.tokens ();
    t.ok (tokens.size () == 4,              "Exclusion 'exc day start 8:30am' --> 3");
    t.is (tokens[0], "exc",                 "Exclusion 'exc day start 8:30am' [0] --> 'exc'");
    t.is (tokens[1], "day",                 "Exclusion 'exc day start 8:30am' [1] --> 'day'");
    t.is (tokens[2], "start",               "Exclusion 'exc day start 8:30am' [2] --> 'start'");
    t.is (tokens[3], "8:30am",              "Exclusion 'exc day start 8:30am' [3] --> '8:30am'");

    // exc day end 1730
    e.initialize ("exc day end 1730");
    tokens = e.tokens ();
    t.ok (tokens.size () == 4,              "Exclusion 'exc day end 1730' --> 3");
    t.is (tokens[0], "exc",                 "Exclusion 'exc day end 1730' [0] --> 'exc'");
    t.is (tokens[1], "day",                 "Exclusion 'exc day end 1730' [1] --> 'day'");
    t.is (tokens[2], "end",                 "Exclusion 'exc day end 1730' [1] --> 'end'");
    t.is (tokens[3], "1730",                "Exclusion 'exc day end 1730' [2] --> '1730'");

    // exc day tue end 3pm
    e.initialize ("exc day tue end 3pm");
    tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc day tue end 3pm' --> 3");
    t.is (tokens[0], "exc",                 "Exclusion 'exc day tue end 3pm' [0] --> 'exc'");
    t.is (tokens[1], "day",                 "Exclusion 'exc day tue end 3pm' [1] --> 'day'");
    t.is (tokens[2], "tue",                 "Exclusion 'exc day tue end 3pm' [2] --> 'tue'");
    t.is (tokens[3], "end",                 "Exclusion 'exc day tue end 3pm' [3] --> 'end'");
    t.is (tokens[4], "3pm",                 "Exclusion 'exc day tue end 3pm' [4] --> '3pm'");
  }

  catch (const std::string& e)
  {
    t.diag (e);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

