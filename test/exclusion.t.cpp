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
  UnitTest t (52);

  try
  {
    // exc monday <8:00:00 12:00:00-12:45:00 >17:30:00
    Exclusion e;
    e.initialize ("exc monday <8:00:00 12:00:00-12:45:00 >17:30:00");
    auto tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc monday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exc",                 "Exclusion 'exc monday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "monday",              "Exclusion 'exc monday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'monday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exc monday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exc monday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exc monday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    e.initialize ("exc tuesday <8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exc",                 "Exclusion 'exc tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "tuesday",             "Exclusion 'exc tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'tuesday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exc tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exc tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exc tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    e.initialize ("exc wednesday <8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exc",                 "Exclusion 'exc wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "wednesday",           "Exclusion 'exc wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'wednesday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exc wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exc wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exc wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    e.initialize ("exc thursday <8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc thursday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exc",                 "Exclusion 'exc thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "thursday",            "Exclusion 'exc thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'thursday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exc thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exc thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exc thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    e.initialize ("exc friday <8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc friday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exc",                 "Exclusion 'exc friday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "friday",              "Exclusion 'exc friday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'friday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exc friday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exc friday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exc friday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    e.initialize ("exc saturday <8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc saturday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exc",                 "Exclusion 'exc saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "saturday",            "Exclusion 'exc saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'saturday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exc saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exc saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exc saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    e.initialize ("exc sunday <8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exc sunday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exc",                 "Exclusion 'exc sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "sunday",              "Exclusion 'exc sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'sunday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exc sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exc sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exc sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    // exc day on 2016-01-01
    e.initialize ("exc day on 2016-01-01");
    tokens = e.tokens ();
    t.ok (tokens.size () == 4,              "Exclusion 'exc day on 2016-01-01' --> 4");
    t.is (tokens[0], "exc",                 "Exclusion 'exc day on 2016-01-01' [0] --> 'exc'");
    t.is (tokens[1], "day",                 "Exclusion 'exc day on 2016-01-01' [1] --> 'day'");
    t.is (tokens[2], "on",                  "Exclusion 'exc day on 2016-01-01' [2] --> 'on'");
    t.is (tokens[3], "2016-01-01",          "Exclusion 'exc day on 2016-01-01' [3] --> '2016-01-01'");

    // exc day off 2016-01-01
    e.initialize ("exc day off 2016-01-01");
    tokens = e.tokens ();
    t.ok (tokens.size () == 4,              "Exclusion 'exc day off 2016-01-01' --> 4");
    t.is (tokens[0], "exc",                 "Exclusion 'exc day off 2016-01-01' [0] --> 'exc'");
    t.is (tokens[1], "day",                 "Exclusion 'exc day off 2016-01-01' [1] --> 'day'");
    t.is (tokens[2], "off",                 "Exclusion 'exc day off 2016-01-01' [2] --> 'off'");
    t.is (tokens[3], "2016-01-01",          "Exclusion 'exc day off 2016-01-01' [3] --> '2016-01-01'");
  }

  catch (const std::string& e)
  {
    t.diag (e);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

