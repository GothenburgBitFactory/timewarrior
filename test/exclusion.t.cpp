////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Paul Beckingham, Federico Hernandez.
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
#include <Range.h>
#include <vector>
#include <iostream>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (261);

  try
  {
    // These tests cover a range from 2015-12-15 to 2016-01-15.
    //
    //    December 2015            January 2016
    // Su Mo Tu We Th Fr Sa    Su Mo Tu We Th Fr Sa
    //        1  2  3  4  5                    1  2
    //  6  7  8  9 10 11 12     3  4  5  6  7  8  9
    // 13 14 15 16 17 18 19    10 11 12 13 14 15 16
    // 20 21 22 23 24 25 26    17 18 19 20 21 22 23
    // 27 28 29 30 31          24 25 26 27 28 29 30
    //                         31
    Range r (Datetime ("2015-12-15"), Datetime ("2016-01-15"));

    // exclusions.monday <8:00:00 12:00:00-12:45:00 >17:30:00
    Exclusion e1 ("exclusions.monday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    auto tokens = e1.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exclusions.monday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.monday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "monday",              "Exclusion 'exclusions.monday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'monday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exclusions.monday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exclusions.monday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exclusions.monday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    auto ranges = e1.ranges (r);
    t.ok (ranges.size () == 12,                                         "Exclusion ranges  --> [12]");
    t.is (ranges[0].start.toISOLocalExtended (), "2015-12-21T00:00:00", "Exclusion range[0].start() --> 2015-12-21T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2015-12-21T08:00:00", "Exclusion range[0].end()   --> 2015-12-21T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2015-12-21T12:00:00", "Exclusion range[1].start() --> 2015-12-21T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2015-12-21T12:45:00", "Exclusion range[1].end()   --> 2015-12-21T12:45:00");
    t.is (ranges[2].start.toISOLocalExtended (), "2015-12-21T17:30:00", "Exclusion range[2].start() --> 2015-12-21T17:30:00");
    t.is (ranges[2].end.toISOLocalExtended (),   "2015-12-22T00:00:00", "Exclusion range[2].end()   --> 2015-12-22T00:00:00");

    t.is (ranges[3].start.toISOLocalExtended (), "2015-12-28T00:00:00", "Exclusion range[3].start() --> 2015-12-28T00:00:00");
    t.is (ranges[3].end.toISOLocalExtended (),   "2015-12-28T08:00:00", "Exclusion range[3].end()   --> 2015-12-28T08:00:00");
    t.is (ranges[4].start.toISOLocalExtended (), "2015-12-28T12:00:00", "Exclusion range[4].start() --> 2015-12-28T12:00:00");
    t.is (ranges[4].end.toISOLocalExtended (),   "2015-12-28T12:45:00", "Exclusion range[4].end()   --> 2015-12-28T12:45:00");
    t.is (ranges[5].start.toISOLocalExtended (), "2015-12-28T17:30:00", "Exclusion range[5].start() --> 2015-12-28T17:30:00");
    t.is (ranges[5].end.toISOLocalExtended (),   "2015-12-29T00:00:00", "Exclusion range[5].end()   --> 2015-12-29T00:00:00");

    t.is (ranges[6].start.toISOLocalExtended (), "2016-01-04T00:00:00", "Exclusion range[6].start() --> 2016-01-04T00:00:00");
    t.is (ranges[6].end.toISOLocalExtended (),   "2016-01-04T08:00:00", "Exclusion range[6].end()   --> 2016-01-04T08:00:00");
    t.is (ranges[7].start.toISOLocalExtended (), "2016-01-04T12:00:00", "Exclusion range[7].start() --> 2016-01-04T12:00:00");
    t.is (ranges[7].end.toISOLocalExtended (),   "2016-01-04T12:45:00", "Exclusion range[7].end()   --> 2016-01-04T12:45:00");
    t.is (ranges[8].start.toISOLocalExtended (), "2016-01-04T17:30:00", "Exclusion range[8].start() --> 2016-01-04T17:30:00");
    t.is (ranges[8].end.toISOLocalExtended (),   "2016-01-05T00:00:00", "Exclusion range[8].end()   --> 2016-01-05T00:00:00");

    t.is (ranges[9].start.toISOLocalExtended (),  "2016-01-11T00:00:00", "Exclusion range[9].start() --> 2016-01-11T00:00:00");
    t.is (ranges[9].end.toISOLocalExtended (),    "2016-01-11T08:00:00", "Exclusion range[9].end()   --> 2016-01-11T08:00:00");
    t.is (ranges[10].start.toISOLocalExtended (), "2016-01-11T12:00:00", "Exclusion range[10].start() --> 2016-01-11T12:00:00");
    t.is (ranges[10].end.toISOLocalExtended (),   "2016-01-11T12:45:00", "Exclusion range[10].end()   --> 2016-01-11T12:45:00");
    t.is (ranges[11].start.toISOLocalExtended (), "2016-01-11T17:30:00", "Exclusion range[11].start() --> 2016-01-11T17:30:00");
    t.is (ranges[11].end.toISOLocalExtended (),   "2016-01-12T00:00:00", "Exclusion range[11].end()   --> 2016-01-12T00:00:00");

    Exclusion e2 ("exclusions.tuesday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e2.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exclusions.tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "tuesday",             "Exclusion 'exclusions.tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'tuesday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exclusions.tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exclusions.tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exclusions.tuesday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    ranges = e2.ranges (r);
    t.ok (ranges.size () == 15,                                         "Exclusion ranges  --> [15]");
    t.is (ranges[0].start.toISOLocalExtended (), "2015-12-15T00:00:00", "Exclusion range[0].start() --> 2015-12-15T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2015-12-15T08:00:00", "Exclusion range[0].end()   --> 2015-12-15T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2015-12-15T12:00:00", "Exclusion range[1].start() --> 2015-12-15T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2015-12-15T12:45:00", "Exclusion range[1].end()   --> 2015-12-15T12:45:00");
    t.is (ranges[2].start.toISOLocalExtended (), "2015-12-15T17:30:00", "Exclusion range[2].start() --> 2015-12-15T17:30:00");
    t.is (ranges[2].end.toISOLocalExtended (),   "2015-12-16T00:00:00", "Exclusion range[2].end()   --> 2015-12-16T00:00:00");

    t.is (ranges[3].start.toISOLocalExtended (), "2015-12-22T00:00:00", "Exclusion range[3].start() --> 2015-12-22T00:00:00");
    t.is (ranges[3].end.toISOLocalExtended (),   "2015-12-22T08:00:00", "Exclusion range[3].end()   --> 2015-12-22T08:00:00");
    t.is (ranges[4].start.toISOLocalExtended (), "2015-12-22T12:00:00", "Exclusion range[4].start() --> 2015-12-22T12:00:00");
    t.is (ranges[4].end.toISOLocalExtended (),   "2015-12-22T12:45:00", "Exclusion range[4].end()   --> 2015-12-22T12:45:00");
    t.is (ranges[5].start.toISOLocalExtended (), "2015-12-22T17:30:00", "Exclusion range[5].start() --> 2015-12-22T17:30:00");
    t.is (ranges[5].end.toISOLocalExtended (),   "2015-12-23T00:00:00", "Exclusion range[5].end()   --> 2015-12-23T00:00:00");

    t.is (ranges[6].start.toISOLocalExtended (), "2015-12-29T00:00:00", "Exclusion range[6].start() --> 2015-12-29T00:00:00");
    t.is (ranges[6].end.toISOLocalExtended (),   "2015-12-29T08:00:00", "Exclusion range[6].end()   --> 2015-12-29T08:00:00");
    t.is (ranges[7].start.toISOLocalExtended (), "2015-12-29T12:00:00", "Exclusion range[7].start() --> 2015-12-29T12:00:00");
    t.is (ranges[7].end.toISOLocalExtended (),   "2015-12-29T12:45:00", "Exclusion range[7].end()   --> 2015-12-29T12:45:00");
    t.is (ranges[8].start.toISOLocalExtended (), "2015-12-29T17:30:00", "Exclusion range[8].start() --> 2015-12-29T17:30:00");
    t.is (ranges[8].end.toISOLocalExtended (),   "2015-12-30T00:00:00", "Exclusion range[8].end()   --> 2015-12-30T00:00:00");

    t.is (ranges[9].start.toISOLocalExtended (),  "2016-01-05T00:00:00", "Exclusion range[9].start() --> 2016-01-05T00:00:00");
    t.is (ranges[9].end.toISOLocalExtended (),    "2016-01-05T08:00:00", "Exclusion range[9].end()   --> 2016-01-05T08:00:00");
    t.is (ranges[10].start.toISOLocalExtended (), "2016-01-05T12:00:00", "Exclusion range[10].start() --> 2016-01-05T12:00:00");
    t.is (ranges[10].end.toISOLocalExtended (),   "2016-01-05T12:45:00", "Exclusion range[10].end()   --> 2016-01-05T12:45:00");
    t.is (ranges[11].start.toISOLocalExtended (), "2016-01-05T17:30:00", "Exclusion range[11].start() --> 2016-01-05T17:30:00");
    t.is (ranges[11].end.toISOLocalExtended (),   "2016-01-06T00:00:00", "Exclusion range[11].end()   --> 2016-01-06T00:00:00");

    t.is (ranges[12].start.toISOLocalExtended (), "2016-01-12T00:00:00", "Exclusion range[12].start() --> 2016-01-12T00:00:00");
    t.is (ranges[12].end.toISOLocalExtended (),   "2016-01-12T08:00:00", "Exclusion range[12].end()   --> 2016-01-12T08:00:00");
    t.is (ranges[13].start.toISOLocalExtended (), "2016-01-12T12:00:00", "Exclusion range[13].start() --> 2016-01-12T12:00:00");
    t.is (ranges[13].end.toISOLocalExtended (),   "2016-01-12T12:45:00", "Exclusion range[13].end()   --> 2016-01-12T12:45:00");
    t.is (ranges[14].start.toISOLocalExtended (), "2016-01-12T17:30:00", "Exclusion range[14].start() --> 2016-01-12T17:30:00");
    t.is (ranges[14].end.toISOLocalExtended (),   "2016-01-13T00:00:00", "Exclusion range[14].end()   --> 2016-01-13T00:00:00");

    Exclusion e3 ("exclusions.wednesday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e3.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exclusions.wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "wednesday",           "Exclusion 'exclusions.wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'wednesday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exclusions.wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exclusions.wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exclusions.wednesday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    ranges = e3.ranges (r);
    t.ok (ranges.size () == 15,                                         "Exclusion ranges  --> [15]");
    t.is (ranges[0].start.toISOLocalExtended (), "2015-12-16T00:00:00", "Exclusion range[0].start() --> 2015-12-16T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2015-12-16T08:00:00", "Exclusion range[0].end()   --> 2015-12-16T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2015-12-16T12:00:00", "Exclusion range[1].start() --> 2015-12-16T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2015-12-16T12:45:00", "Exclusion range[1].end()   --> 2015-12-16T12:45:00");
    t.is (ranges[2].start.toISOLocalExtended (), "2015-12-16T17:30:00", "Exclusion range[2].start() --> 2015-12-16T17:30:00");
    t.is (ranges[2].end.toISOLocalExtended (),   "2015-12-17T00:00:00", "Exclusion range[2].end()   --> 2015-12-17T00:00:00");

    t.is (ranges[3].start.toISOLocalExtended (), "2015-12-23T00:00:00", "Exclusion range[3].start() --> 2015-12-23T00:00:00");
    t.is (ranges[3].end.toISOLocalExtended (),   "2015-12-23T08:00:00", "Exclusion range[3].end()   --> 2015-12-23T08:00:00");
    t.is (ranges[4].start.toISOLocalExtended (), "2015-12-23T12:00:00", "Exclusion range[4].start() --> 2015-12-23T12:00:00");
    t.is (ranges[4].end.toISOLocalExtended (),   "2015-12-23T12:45:00", "Exclusion range[4].end()   --> 2015-12-23T12:45:00");
    t.is (ranges[5].start.toISOLocalExtended (), "2015-12-23T17:30:00", "Exclusion range[5].start() --> 2015-12-23T17:30:00");
    t.is (ranges[5].end.toISOLocalExtended (),   "2015-12-24T00:00:00", "Exclusion range[5].end()   --> 2015-12-24T00:00:00");

    t.is (ranges[6].start.toISOLocalExtended (), "2015-12-30T00:00:00", "Exclusion range[6].start() --> 2015-12-30T00:00:00");
    t.is (ranges[6].end.toISOLocalExtended (),   "2015-12-30T08:00:00", "Exclusion range[6].end()   --> 2015-12-30T08:00:00");
    t.is (ranges[7].start.toISOLocalExtended (), "2015-12-30T12:00:00", "Exclusion range[7].start() --> 2015-12-30T12:00:00");
    t.is (ranges[7].end.toISOLocalExtended (),   "2015-12-30T12:45:00", "Exclusion range[7].end()   --> 2015-12-30T12:45:00");
    t.is (ranges[8].start.toISOLocalExtended (), "2015-12-30T17:30:00", "Exclusion range[8].start() --> 2015-12-30T17:30:00");
    t.is (ranges[8].end.toISOLocalExtended (),   "2015-12-31T00:00:00", "Exclusion range[8].end()   --> 2015-12-31T00:00:00");

    t.is (ranges[9].start.toISOLocalExtended (),  "2016-01-06T00:00:00", "Exclusion range[9].start() --> 2016-01-06T00:00:00");
    t.is (ranges[9].end.toISOLocalExtended (),    "2016-01-06T08:00:00", "Exclusion range[9].end()   --> 2016-01-06T08:00:00");
    t.is (ranges[10].start.toISOLocalExtended (), "2016-01-06T12:00:00", "Exclusion range[10].start() --> 2016-01-06T12:00:00");
    t.is (ranges[10].end.toISOLocalExtended (),   "2016-01-06T12:45:00", "Exclusion range[10].end()   --> 2016-01-06T12:45:00");
    t.is (ranges[11].start.toISOLocalExtended (), "2016-01-06T17:30:00", "Exclusion range[11].start() --> 2016-01-06T17:30:00");
    t.is (ranges[11].end.toISOLocalExtended (),   "2016-01-07T00:00:00", "Exclusion range[11].end()   --> 2016-01-07T00:00:00");

    t.is (ranges[12].start.toISOLocalExtended (), "2016-01-13T00:00:00", "Exclusion range[12].start() --> 2016-01-13T00:00:00");
    t.is (ranges[12].end.toISOLocalExtended (),   "2016-01-13T08:00:00", "Exclusion range[12].end()   --> 2016-01-13T08:00:00");
    t.is (ranges[13].start.toISOLocalExtended (), "2016-01-13T12:00:00", "Exclusion range[13].start() --> 2016-01-13T12:00:00");
    t.is (ranges[13].end.toISOLocalExtended (),   "2016-01-13T12:45:00", "Exclusion range[13].end()   --> 2016-01-13T12:45:00");
    t.is (ranges[14].start.toISOLocalExtended (), "2016-01-13T17:30:00", "Exclusion range[14].start() --> 2016-01-13T17:30:00");
    t.is (ranges[14].end.toISOLocalExtended (),   "2016-01-14T00:00:00", "Exclusion range[14].end()   --> 2016-01-14T00:00:00");

    Exclusion e4 ("exclusions.thursday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e4.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exclusions.thursday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "thursday",            "Exclusion 'exclusions.thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'thursday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exclusions.thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exclusions.thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exclusions.thursday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    ranges = e4.ranges (r);
    t.ok (ranges.size () == 15,                                         "Exclusion ranges  --> [15]");
    t.is (ranges[0].start.toISOLocalExtended (), "2015-12-17T00:00:00", "Exclusion range[0].start() --> 2015-12-17T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2015-12-17T08:00:00", "Exclusion range[0].end()   --> 2015-12-17T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2015-12-17T12:00:00", "Exclusion range[1].start() --> 2015-12-17T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2015-12-17T12:45:00", "Exclusion range[1].end()   --> 2015-12-17T12:45:00");
    t.is (ranges[2].start.toISOLocalExtended (), "2015-12-17T17:30:00", "Exclusion range[2].start() --> 2015-12-17T17:30:00");
    t.is (ranges[2].end.toISOLocalExtended (),   "2015-12-18T00:00:00", "Exclusion range[2].end()   --> 2015-12-18T00:00:00");

    t.is (ranges[3].start.toISOLocalExtended (), "2015-12-24T00:00:00", "Exclusion range[3].start() --> 2015-12-24T00:00:00");
    t.is (ranges[3].end.toISOLocalExtended (),   "2015-12-24T08:00:00", "Exclusion range[3].end()   --> 2015-12-24T08:00:00");
    t.is (ranges[4].start.toISOLocalExtended (), "2015-12-24T12:00:00", "Exclusion range[4].start() --> 2015-12-24T12:00:00");
    t.is (ranges[4].end.toISOLocalExtended (),   "2015-12-24T12:45:00", "Exclusion range[4].end()   --> 2015-12-24T12:45:00");
    t.is (ranges[5].start.toISOLocalExtended (), "2015-12-24T17:30:00", "Exclusion range[5].start() --> 2015-12-24T17:30:00");
    t.is (ranges[5].end.toISOLocalExtended (),   "2015-12-25T00:00:00", "Exclusion range[5].end()   --> 2015-12-25T00:00:00");

    t.is (ranges[6].start.toISOLocalExtended (), "2015-12-31T00:00:00", "Exclusion range[6].start() --> 2015-12-31T00:00:00");
    t.is (ranges[6].end.toISOLocalExtended (),   "2015-12-31T08:00:00", "Exclusion range[6].end()   --> 2015-12-31T08:00:00");
    t.is (ranges[7].start.toISOLocalExtended (), "2015-12-31T12:00:00", "Exclusion range[7].start() --> 2015-12-31T12:00:00");
    t.is (ranges[7].end.toISOLocalExtended (),   "2015-12-31T12:45:00", "Exclusion range[7].end()   --> 2015-12-31T12:45:00");
    t.is (ranges[8].start.toISOLocalExtended (), "2015-12-31T17:30:00", "Exclusion range[8].start() --> 2015-12-31T17:30:00");
    t.is (ranges[8].end.toISOLocalExtended (),   "2016-01-01T00:00:00", "Exclusion range[8].end()   --> 2016-01-01T00:00:00");

    t.is (ranges[9].start.toISOLocalExtended (),  "2016-01-07T00:00:00", "Exclusion range[9].start() --> 2016-01-07T00:00:00");
    t.is (ranges[9].end.toISOLocalExtended (),    "2016-01-07T08:00:00", "Exclusion range[9].end()   --> 2016-01-07T08:00:00");
    t.is (ranges[10].start.toISOLocalExtended (), "2016-01-07T12:00:00", "Exclusion range[10].start() --> 2016-01-07T12:00:00");
    t.is (ranges[10].end.toISOLocalExtended (),   "2016-01-07T12:45:00", "Exclusion range[10].end()   --> 2016-01-07T12:45:00");
    t.is (ranges[11].start.toISOLocalExtended (), "2016-01-07T17:30:00", "Exclusion range[11].start() --> 2016-01-07T17:30:00");
    t.is (ranges[11].end.toISOLocalExtended (),   "2016-01-08T00:00:00", "Exclusion range[11].end()   --> 2016-01-08T00:00:00");

    t.is (ranges[12].start.toISOLocalExtended (), "2016-01-14T00:00:00", "Exclusion range[12].start() --> 2016-01-14T00:00:00");
    t.is (ranges[12].end.toISOLocalExtended (),   "2016-01-14T08:00:00", "Exclusion range[12].end()   --> 2016-01-14T08:00:00");
    t.is (ranges[13].start.toISOLocalExtended (), "2016-01-14T12:00:00", "Exclusion range[13].start() --> 2016-01-14T12:00:00");
    t.is (ranges[13].end.toISOLocalExtended (),   "2016-01-14T12:45:00", "Exclusion range[13].end()   --> 2016-01-14T12:45:00");
    t.is (ranges[14].start.toISOLocalExtended (), "2016-01-14T17:30:00", "Exclusion range[14].start() --> 2016-01-14T17:30:00");
    t.is (ranges[14].end.toISOLocalExtended (),   "2016-01-15T00:00:00", "Exclusion range[14].end()   --> 2016-01-15T00:00:00");

    Exclusion e5 ("exclusions.friday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e5.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exclusions.friday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.friday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "friday",              "Exclusion 'exclusions.friday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'friday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exclusions.friday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exclusions.friday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exclusions.friday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    ranges = e5.ranges (r);
    t.ok (ranges.size () == 12,                                            "Exclusion ranges  --> [12]");
    t.is (ranges[0].start.toISOLocalExtended (), "2015-12-18T00:00:00", "Exclusion range[0].start() --> 2015-12-18T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2015-12-18T08:00:00", "Exclusion range[0].end()   --> 2015-12-18T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2015-12-18T12:00:00", "Exclusion range[1].start() --> 2015-12-18T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2015-12-18T12:45:00", "Exclusion range[1].end()   --> 2015-12-18T12:45:00");
    t.is (ranges[2].start.toISOLocalExtended (), "2015-12-18T17:30:00", "Exclusion range[2].start() --> 2015-12-18T17:30:00");
    t.is (ranges[2].end.toISOLocalExtended (),   "2015-12-19T00:00:00", "Exclusion range[2].end()   --> 2015-12-19T00:00:00");

    t.is (ranges[3].start.toISOLocalExtended (), "2015-12-25T00:00:00", "Exclusion range[3].start() --> 2015-12-25T00:00:00");
    t.is (ranges[3].end.toISOLocalExtended (),   "2015-12-25T08:00:00", "Exclusion range[3].end()   --> 2015-12-25T08:00:00");
    t.is (ranges[4].start.toISOLocalExtended (), "2015-12-25T12:00:00", "Exclusion range[4].start() --> 2015-12-25T12:00:00");
    t.is (ranges[4].end.toISOLocalExtended (),   "2015-12-25T12:45:00", "Exclusion range[4].end()   --> 2015-12-25T12:45:00");
    t.is (ranges[5].start.toISOLocalExtended (), "2015-12-25T17:30:00", "Exclusion range[5].start() --> 2015-12-25T17:30:00");
    t.is (ranges[5].end.toISOLocalExtended (),   "2015-12-26T00:00:00", "Exclusion range[5].end()   --> 2015-12-26T00:00:00");

    t.is (ranges[6].start.toISOLocalExtended (), "2016-01-01T00:00:00", "Exclusion range[6].start() --> 2016-01-01T00:00:00");
    t.is (ranges[6].end.toISOLocalExtended (),   "2016-01-01T08:00:00", "Exclusion range[6].end()   --> 2016-01-01T08:00:00");
    t.is (ranges[7].start.toISOLocalExtended (), "2016-01-01T12:00:00", "Exclusion range[7].start() --> 2016-01-01T12:00:00");
    t.is (ranges[7].end.toISOLocalExtended (),   "2016-01-01T12:45:00", "Exclusion range[7].end()   --> 2016-01-01T12:45:00");
    t.is (ranges[8].start.toISOLocalExtended (), "2016-01-01T17:30:00", "Exclusion range[8].start() --> 2016-01-01T17:30:00");
    t.is (ranges[8].end.toISOLocalExtended (),   "2016-01-02T00:00:00", "Exclusion range[8].end()   --> 2016-01-02T00:00:00");

    t.is (ranges[9].start.toISOLocalExtended (),  "2016-01-08T00:00:00", "Exclusion range[9].start() --> 2016-01-08T00:00:00");
    t.is (ranges[9].end.toISOLocalExtended (),    "2016-01-08T08:00:00", "Exclusion range[9].end()   --> 2016-01-08T08:00:00");
    t.is (ranges[10].start.toISOLocalExtended (), "2016-01-08T12:00:00", "Exclusion range[10].start() --> 2016-01-08T12:00:00");
    t.is (ranges[10].end.toISOLocalExtended (),   "2016-01-08T12:45:00", "Exclusion range[10].end()   --> 2016-01-08T12:45:00");
    t.is (ranges[11].start.toISOLocalExtended (), "2016-01-08T17:30:00", "Exclusion range[11].start() --> 2016-01-08T17:30:00");
    t.is (ranges[11].end.toISOLocalExtended (),   "2016-01-09T00:00:00", "Exclusion range[11].end()   --> 2016-01-09T00:00:00");

    Exclusion e6 ("exclusions.saturday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e6.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exclusions.saturday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "saturday",            "Exclusion 'exclusions.saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'saturday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exclusions.saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exclusions.saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exclusions.saturday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    ranges = e6.ranges (r);
    t.ok (ranges.size () == 12,                                            "Exclusion ranges  --> [12]");
    t.is (ranges[0].start.toISOLocalExtended (), "2015-12-19T00:00:00", "Exclusion range[0].start() --> 2015-12-19T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2015-12-19T08:00:00", "Exclusion range[0].end()   --> 2015-12-19T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2015-12-19T12:00:00", "Exclusion range[1].start() --> 2015-12-19T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2015-12-19T12:45:00", "Exclusion range[1].end()   --> 2015-12-19T12:45:00");
    t.is (ranges[2].start.toISOLocalExtended (), "2015-12-19T17:30:00", "Exclusion range[2].start() --> 2015-12-19T17:30:00");
    t.is (ranges[2].end.toISOLocalExtended (),   "2015-12-20T00:00:00", "Exclusion range[2].end()   --> 2015-12-20T00:00:00");

    t.is (ranges[3].start.toISOLocalExtended (), "2015-12-26T00:00:00", "Exclusion range[3].start() --> 2015-12-26T00:00:00");
    t.is (ranges[3].end.toISOLocalExtended (),   "2015-12-26T08:00:00", "Exclusion range[3].end()   --> 2015-12-26T08:00:00");
    t.is (ranges[4].start.toISOLocalExtended (), "2015-12-26T12:00:00", "Exclusion range[4].start() --> 2015-12-26T12:00:00");
    t.is (ranges[4].end.toISOLocalExtended (),   "2015-12-26T12:45:00", "Exclusion range[4].end()   --> 2015-12-26T12:45:00");
    t.is (ranges[5].start.toISOLocalExtended (), "2015-12-26T17:30:00", "Exclusion range[5].start() --> 2015-12-26T17:30:00");
    t.is (ranges[5].end.toISOLocalExtended (),   "2015-12-27T00:00:00", "Exclusion range[5].end()   --> 2015-12-27T00:00:00");

    t.is (ranges[6].start.toISOLocalExtended (), "2016-01-02T00:00:00", "Exclusion range[6].start() --> 2016-01-02T00:00:00");
    t.is (ranges[6].end.toISOLocalExtended (),   "2016-01-02T08:00:00", "Exclusion range[6].end()   --> 2016-01-02T08:00:00");
    t.is (ranges[7].start.toISOLocalExtended (), "2016-01-02T12:00:00", "Exclusion range[7].start() --> 2016-01-02T12:00:00");
    t.is (ranges[7].end.toISOLocalExtended (),   "2016-01-02T12:45:00", "Exclusion range[7].end()   --> 2016-01-02T12:45:00");
    t.is (ranges[8].start.toISOLocalExtended (), "2016-01-02T17:30:00", "Exclusion range[8].start() --> 2016-01-02T17:30:00");
    t.is (ranges[8].end.toISOLocalExtended (),   "2016-01-03T00:00:00", "Exclusion range[8].end()   --> 2016-01-03T00:00:00");

    t.is (ranges[9].start.toISOLocalExtended (),  "2016-01-09T00:00:00", "Exclusion range[9].start() --> 2016-01-09T00:00:00");
    t.is (ranges[9].end.toISOLocalExtended (),    "2016-01-09T08:00:00", "Exclusion range[9].end()   --> 2016-01-09T08:00:00");
    t.is (ranges[10].start.toISOLocalExtended (), "2016-01-09T12:00:00", "Exclusion range[10].start() --> 2016-01-09T12:00:00");
    t.is (ranges[10].end.toISOLocalExtended (),   "2016-01-09T12:45:00", "Exclusion range[10].end()   --> 2016-01-09T12:45:00");
    t.is (ranges[11].start.toISOLocalExtended (), "2016-01-09T17:30:00", "Exclusion range[11].start() --> 2016-01-09T17:30:00");
    t.is (ranges[11].end.toISOLocalExtended (),   "2016-01-10T00:00:00", "Exclusion range[11].end()   --> 2016-01-10T00:00:00");

    Exclusion e7 ("exclusions.sunday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    tokens = e7.tokens ();
    t.ok (tokens.size () == 5,              "Exclusion 'exclusions.sunday <8:00:00 12:00:00-12:45:00 >17:30:00' --> 5");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [0] --> 'exc'");
    t.is (tokens[1], "sunday",              "Exclusion 'exclusions.sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [1] --> 'sunday'");
    t.is (tokens[2], "<8:00:00",            "Exclusion 'exclusions.sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [2] --> '<8:00:00'");
    t.is (tokens[3], "12:00:00-12:45:00",   "Exclusion 'exclusions.sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [3] --> '12:00:00-12:45:00'");
    t.is (tokens[4], ">17:30:00",           "Exclusion 'exclusions.sunday <8:00:00 12:00:00-12:45:00 >17:30:00' [4] --> '>17:30:00'");

    ranges = e7.ranges (r);
    t.ok (ranges.size () == 12,                                         "Exclusion ranges  --> [12]");
    t.is (ranges[0].start.toISOLocalExtended (), "2015-12-20T00:00:00", "Exclusion range[0].start() --> 2015-12-20T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2015-12-20T08:00:00", "Exclusion range[0].end()   --> 2015-12-20T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2015-12-20T12:00:00", "Exclusion range[1].start() --> 2015-12-20T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2015-12-20T12:45:00", "Exclusion range[1].end()   --> 2015-12-20T12:45:00");
    t.is (ranges[2].start.toISOLocalExtended (), "2015-12-20T17:30:00", "Exclusion range[2].start() --> 2015-12-20T17:30:00");
    t.is (ranges[2].end.toISOLocalExtended (),   "2015-12-21T00:00:00", "Exclusion range[2].end()   --> 2015-12-21T00:00:00");

    t.is (ranges[3].start.toISOLocalExtended (), "2015-12-27T00:00:00", "Exclusion range[3].start() --> 2015-12-27T00:00:00");
    t.is (ranges[3].end.toISOLocalExtended (),   "2015-12-27T08:00:00", "Exclusion range[3].end()   --> 2015-12-27T08:00:00");
    t.is (ranges[4].start.toISOLocalExtended (), "2015-12-27T12:00:00", "Exclusion range[4].start() --> 2015-12-27T12:00:00");
    t.is (ranges[4].end.toISOLocalExtended (),   "2015-12-27T12:45:00", "Exclusion range[4].end()   --> 2015-12-27T12:45:00");
    t.is (ranges[5].start.toISOLocalExtended (), "2015-12-27T17:30:00", "Exclusion range[5].start() --> 2015-12-22717:30:00");
    t.is (ranges[5].end.toISOLocalExtended (),   "2015-12-28T00:00:00", "Exclusion range[5].end()   --> 2015-12-28T00:00:00");

    t.is (ranges[6].start.toISOLocalExtended (), "2016-01-03T00:00:00", "Exclusion range[6].start() --> 2016-01-03T00:00:00");
    t.is (ranges[6].end.toISOLocalExtended (),   "2016-01-03T08:00:00", "Exclusion range[6].end()   --> 2016-01-03T08:00:00");
    t.is (ranges[7].start.toISOLocalExtended (), "2016-01-03T12:00:00", "Exclusion range[7].start() --> 2016-01-03T12:00:00");
    t.is (ranges[7].end.toISOLocalExtended (),   "2016-01-03T12:45:00", "Exclusion range[7].end()   --> 2016-01-03T12:45:00");
    t.is (ranges[8].start.toISOLocalExtended (), "2016-01-03T17:30:00", "Exclusion range[8].start() --> 2016-01-03T17:30:00");
    t.is (ranges[8].end.toISOLocalExtended (),   "2016-01-04T00:00:00", "Exclusion range[8].end()   --> 2016-01-04T00:00:00");

    t.is (ranges[9].start.toISOLocalExtended (),  "2016-01-10T00:00:00", "Exclusion range[9].start() --> 2016-01-10T00:00:00");
    t.is (ranges[9].end.toISOLocalExtended (),    "2016-01-10T08:00:00", "Exclusion range[9].end()   --> 2016-01-10T08:00:00");
    t.is (ranges[10].start.toISOLocalExtended (), "2016-01-10T12:00:00", "Exclusion range[10].start() --> 2016-01-10T12:00:00");
    t.is (ranges[10].end.toISOLocalExtended (),   "2016-01-10T12:45:00", "Exclusion range[10].end()   --> 2016-01-10T12:45:00");
    t.is (ranges[11].start.toISOLocalExtended (), "2016-01-10T17:30:00", "Exclusion range[11].start() --> 2016-01-10T17:30:00");
    t.is (ranges[11].end.toISOLocalExtended (),   "2016-01-11T00:00:00", "Exclusion range[11].end()   --> 2016-01-11T00:00:00");

    // exclusions.days.2016-01-01 on
    Exclusion e8 ("exclusions.days.2016_01_01", "on");
    tokens = e8.tokens ();
    t.ok (tokens.size () == 4,              "Exclusion 'exclusions.days on 2016_01_01' --> 4");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.days on 2016_01_01' [0] --> 'exclusions'");
    t.is (tokens[1], "days",                "Exclusion 'exclusions.days on 2016_01_01' [1] --> 'days'");
    t.is (tokens[2], "2016_01_01",          "Exclusion 'exclusions.days on 2016_01_01' [2] --> '2016_01_01'");
    t.is (tokens[3], "on",                  "Exclusion 'exclusions.days on 2016_01_01' [3] --> 'on'");

    ranges = e8.ranges (r);
    t.ok (ranges.size () == 1,                              "Exclusion ranges --> [1]");
    t.is (ranges[0].start.toString ("Y-M-D"), "2016-01-01", "Exclusion range[0].start() --> 2016-01-01");
    t.is (ranges[0].end.toString ("Y-M-D"),   "2016-01-02", "Exclusion range[0].end() --> 2016-01-02");
    t.ok (e8.additive (),                                   "Exclusion 'day on ...' --> additive");

    // exclusions.days off 2016-01-01
    Exclusion e9 ("exclusions.days.2016_01_01", "off");
    tokens = e9.tokens ();
    t.ok (tokens.size () == 4,              "Exclusion 'exclusions.days off 2016_01_01' --> 4");
    t.is (tokens[0], "exclusions",          "Exclusion 'exclusions.days off 2016_01_01' [0] --> 'exclusions'");
    t.is (tokens[1], "days",                "Exclusion 'exclusions.days off 2016_01_01' [1] --> 'days'");
    t.is (tokens[2], "2016_01_01",          "Exclusion 'exclusions.days off 2016_01_01' [2] --> '2016_01_01'");
    t.is (tokens[3], "off",                 "Exclusion 'exclusions.days off 2016_01_01' [3] --> 'off'");

    ranges = e9.ranges (r);
    t.ok (ranges.size () == 1,                              "Exclusion ranges --> [1]");
    t.is (ranges[0].start.toString ("Y-M-D"), "2016-01-01", "Exclusion range[0].start() --> 2016-01-01");
    t.is (ranges[0].end.toString ("Y-M-D"),   "2016-01-02", "Exclusion range[0].end() --> 2016-01-02");
    t.notok (e9.additive (),                                "Exclusion 'day off ...' --> !additive");

    // Test day iteration for Exclusion::ranges.
    // The key part of this test is that the end is < 24 hours from the start,
    // which revealed a bug where Exclusion::ranges iterated one day at a time
    // from 'start' to 'end', and never yielded results for the second day.
    Exclusion e10 ("exclusions.thursday", "<8:00:00 12:00:00-12:45:00 >17:30:00");
    Range r1d {{"20160512T160000"}, {"20160513T125000"}};
    ranges = e10.ranges (r1d);
    t.ok (ranges.size () == 1,                                          "Exclusion ranges --> [1]");
    t.is (ranges[0].start.toISOLocalExtended (), "2016-05-12T17:30:00", "Exclusion range[2].start() --> 2016-05-12T17:30:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2016-05-13T00:00:00", "Exclusion range[2].end()   --> 2016-05-13T00:00:00");

    Exclusion e11 ("exclusions.friday",   "<8:00:00 12:00:00-12:45:00 >17:30:00");
    ranges = e11.ranges (r1d);
    t.ok (ranges.size () == 2,                                          "Exclusion ranges --> [2]");
    t.is (ranges[0].start.toISOLocalExtended (), "2016-05-13T00:00:00", "Exclusion range[0].start() --> 2016-05-13T00:00:00");
    t.is (ranges[0].end.toISOLocalExtended (),   "2016-05-13T08:00:00", "Exclusion range[0].end()   --> 2016-05-13T08:00:00");
    t.is (ranges[1].start.toISOLocalExtended (), "2016-05-13T12:00:00", "Exclusion range[1].start() --> 2016-05-13T12:00:00");
    t.is (ranges[1].end.toISOLocalExtended (),   "2016-05-13T12:45:00", "Exclusion range[1].end()   --> 2016-05-13T12:45:00");
  }

  catch (const std::string& e)
  {
    t.diag (e);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

