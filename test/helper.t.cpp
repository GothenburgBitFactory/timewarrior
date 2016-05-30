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
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (68);

  // int quantizeTo15Minutes (const int);
  t.is (quantizeTo15Minutes (0),   0, "quantizeTo15Minutes 0 --> 0");
  t.is (quantizeTo15Minutes (1),   0, "quantizeTo15Minutes 1 --> 0");
  t.is (quantizeTo15Minutes (2),   0, "quantizeTo15Minutes 2 --> 0");
  t.is (quantizeTo15Minutes (3),   0, "quantizeTo15Minutes 3 --> 0");
  t.is (quantizeTo15Minutes (4),   0, "quantizeTo15Minutes 4 --> 0");
  t.is (quantizeTo15Minutes (5),   0, "quantizeTo15Minutes 5 --> 0");
  t.is (quantizeTo15Minutes (6),   0, "quantizeTo15Minutes 6 --> 0");
  t.is (quantizeTo15Minutes (7),   0, "quantizeTo15Minutes 7 --> 0");
  t.is (quantizeTo15Minutes (8),  15, "quantizeTo15Minutes 8 --> 15");
  t.is (quantizeTo15Minutes (9),  15, "quantizeTo15Minutes 9 --> 15");
  t.is (quantizeTo15Minutes (10), 15, "quantizeTo15Minutes 10 --> 15");
  t.is (quantizeTo15Minutes (11), 15, "quantizeTo15Minutes 11 --> 15");
  t.is (quantizeTo15Minutes (12), 15, "quantizeTo15Minutes 12 --> 15");
  t.is (quantizeTo15Minutes (13), 15, "quantizeTo15Minutes 13 --> 15");
  t.is (quantizeTo15Minutes (14), 15, "quantizeTo15Minutes 14 --> 15");
  t.is (quantizeTo15Minutes (15), 15, "quantizeTo15Minutes 15 --> 15");
  t.is (quantizeTo15Minutes (16), 15, "quantizeTo15Minutes 16 --> 15");
  t.is (quantizeTo15Minutes (17), 15, "quantizeTo15Minutes 17 --> 15");
  t.is (quantizeTo15Minutes (18), 15, "quantizeTo15Minutes 18 --> 15");
  t.is (quantizeTo15Minutes (19), 15, "quantizeTo15Minutes 19 --> 15");
  t.is (quantizeTo15Minutes (20), 15, "quantizeTo15Minutes 20 --> 15");
  t.is (quantizeTo15Minutes (21), 15, "quantizeTo15Minutes 21 --> 15");
  t.is (quantizeTo15Minutes (22), 15, "quantizeTo15Minutes 22 --> 15");
  t.is (quantizeTo15Minutes (23), 30, "quantizeTo15Minutes 23 --> 30");
  t.is (quantizeTo15Minutes (24), 30, "quantizeTo15Minutes 24 --> 30");
  t.is (quantizeTo15Minutes (25), 30, "quantizeTo15Minutes 25 --> 30");
  t.is (quantizeTo15Minutes (26), 30, "quantizeTo15Minutes 26 --> 30");
  t.is (quantizeTo15Minutes (27), 30, "quantizeTo15Minutes 27 --> 30");
  t.is (quantizeTo15Minutes (28), 30, "quantizeTo15Minutes 28 --> 30");
  t.is (quantizeTo15Minutes (29), 30, "quantizeTo15Minutes 29 --> 30");
  t.is (quantizeTo15Minutes (30), 30, "quantizeTo15Minutes 30 --> 30");
  t.is (quantizeTo15Minutes (31), 30, "quantizeTo15Minutes 31 --> 30");
  t.is (quantizeTo15Minutes (32), 30, "quantizeTo15Minutes 32 --> 30");
  t.is (quantizeTo15Minutes (33), 30, "quantizeTo15Minutes 33 --> 30");
  t.is (quantizeTo15Minutes (34), 30, "quantizeTo15Minutes 34 --> 30");
  t.is (quantizeTo15Minutes (35), 30, "quantizeTo15Minutes 35 --> 30");
  t.is (quantizeTo15Minutes (36), 30, "quantizeTo15Minutes 36 --> 30");
  t.is (quantizeTo15Minutes (37), 30, "quantizeTo15Minutes 37 --> 30");
  t.is (quantizeTo15Minutes (38), 45, "quantizeTo15Minutes 38 --> 45");
  t.is (quantizeTo15Minutes (39), 45, "quantizeTo15Minutes 39 --> 45");
  t.is (quantizeTo15Minutes (40), 45, "quantizeTo15Minutes 40 --> 45");
  t.is (quantizeTo15Minutes (41), 45, "quantizeTo15Minutes 41 --> 45");
  t.is (quantizeTo15Minutes (42), 45, "quantizeTo15Minutes 42 --> 45");
  t.is (quantizeTo15Minutes (43), 45, "quantizeTo15Minutes 43 --> 45");
  t.is (quantizeTo15Minutes (44), 45, "quantizeTo15Minutes 44 --> 45");
  t.is (quantizeTo15Minutes (45), 45, "quantizeTo15Minutes 45 --> 45");
  t.is (quantizeTo15Minutes (46), 45, "quantizeTo15Minutes 46 --> 45");
  t.is (quantizeTo15Minutes (47), 45, "quantizeTo15Minutes 47 --> 45");
  t.is (quantizeTo15Minutes (48), 45, "quantizeTo15Minutes 48 --> 45");
  t.is (quantizeTo15Minutes (49), 45, "quantizeTo15Minutes 49 --> 45");
  t.is (quantizeTo15Minutes (50), 45, "quantizeTo15Minutes 50 --> 45");
  t.is (quantizeTo15Minutes (51), 45, "quantizeTo15Minutes 51 --> 45");
  t.is (quantizeTo15Minutes (52), 45, "quantizeTo15Minutes 52 --> 45");
  t.is (quantizeTo15Minutes (53), 60, "quantizeTo15Minutes 53 --> 60");
  t.is (quantizeTo15Minutes (54), 60, "quantizeTo15Minutes 54 --> 60");
  t.is (quantizeTo15Minutes (55), 60, "quantizeTo15Minutes 55 --> 60");
  t.is (quantizeTo15Minutes (56), 60, "quantizeTo15Minutes 56 --> 60");
  t.is (quantizeTo15Minutes (57), 60, "quantizeTo15Minutes 57 --> 60");
  t.is (quantizeTo15Minutes (58), 60, "quantizeTo15Minutes 58 --> 60");
  t.is (quantizeTo15Minutes (59), 60, "quantizeTo15Minutes 59 --> 60");
  t.is (quantizeTo15Minutes (60), 60, "quantizeTo15Minutes 60 --> 60");

  // std::string minimalDelta (const Datetime&, const Datetime&);
  t.is (minimalDelta ({"2016-05-30T15:11:01"}, {"2017-01-01T00:00:00"}), "2017-01-01T00:00:00", "minimalDelta 2016-05-30T15:11:01/2017-01-01T00:00:00 --> 2017-01-01T00:00:00");
  t.is (minimalDelta ({"2016-05-30T15:11:01"}, {"2016-01-01T00:00:00"}), "     01-01T00:00:00", "minimalDelta 2016-05-30T15:11:01/2016-01-01T00:00:00 -->      01-01T00:00:00");
  t.is (minimalDelta ({"2016-05-30T15:11:01"}, {"2016-05-01T00:00:00"}), "        01T00:00:00", "minimalDelta 2016-05-30T15:11:01/2016-05-01T00:00:00 -->         01T00:00:00");
  t.is (minimalDelta ({"2016-05-30T15:11:01"}, {"2016-05-30T00:00:00"}), "           00:00:00", "minimalDelta 2016-05-30T15:11:01/2016-05-30T00:00:00 -->            00:00:00");
  t.is (minimalDelta ({"2016-05-30T15:11:01"}, {"2016-05-30T15:00:00"}), "              00:00", "minimalDelta 2016-05-30T15:11:01/2016-05-30T15:00:00 -->               00:00");
  t.is (minimalDelta ({"2016-05-30T15:11:01"}, {"2016-05-30T15:11:00"}), "                 00", "minimalDelta 2016-05-30T15:11:01/2016-05-30T15:11:00 -->                  00");
  t.is (minimalDelta ({"2016-05-30T15:11:01"}, {"2016-05-30T15:11:01"}), "                 01", "minimalDelta 2016-05-30T15:11:01/2016-05-30T15:11:01 -->                  01");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

