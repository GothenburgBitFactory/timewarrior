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
#include <timew.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (129);

  // int quantizeToNMinutes (const int);
  t.is (quantizeToNMinutes (0, 15),   0, "quantizeToNMinutes 0/15 --> 0");
  t.is (quantizeToNMinutes (1, 15),   0, "quantizeToNMinutes 1/15 --> 0");
  t.is (quantizeToNMinutes (2, 15),   0, "quantizeToNMinutes 2/15 --> 0");
  t.is (quantizeToNMinutes (3, 15),   0, "quantizeToNMinutes 3/15 --> 0");
  t.is (quantizeToNMinutes (4, 15),   0, "quantizeToNMinutes 4/15 --> 0");
  t.is (quantizeToNMinutes (5, 15),   0, "quantizeToNMinutes 5/15 --> 0");
  t.is (quantizeToNMinutes (6, 15),   0, "quantizeToNMinutes 6/15 --> 0");
  t.is (quantizeToNMinutes (7, 15),  15, "quantizeToNMinutes 7/15 --> 15");
  t.is (quantizeToNMinutes (8, 15),  15, "quantizeToNMinutes 8/15 --> 15");
  t.is (quantizeToNMinutes (9, 15),  15, "quantizeToNMinutes 9/15 --> 15");
  t.is (quantizeToNMinutes (10, 15), 15, "quantizeToNMinutes 10/15 --> 15");
  t.is (quantizeToNMinutes (11, 15), 15, "quantizeToNMinutes 11/15 --> 15");
  t.is (quantizeToNMinutes (12, 15), 15, "quantizeToNMinutes 12/15 --> 15");
  t.is (quantizeToNMinutes (13, 15), 15, "quantizeToNMinutes 13/15 --> 15");
  t.is (quantizeToNMinutes (14, 15), 15, "quantizeToNMinutes 14/15 --> 15");
  t.is (quantizeToNMinutes (15, 15), 15, "quantizeToNMinutes 15/15 --> 15");
  t.is (quantizeToNMinutes (16, 15), 15, "quantizeToNMinutes 16/15 --> 15");
  t.is (quantizeToNMinutes (17, 15), 15, "quantizeToNMinutes 17/15 --> 15");
  t.is (quantizeToNMinutes (18, 15), 15, "quantizeToNMinutes 18/15 --> 15");
  t.is (quantizeToNMinutes (19, 15), 15, "quantizeToNMinutes 19/15 --> 15");
  t.is (quantizeToNMinutes (20, 15), 15, "quantizeToNMinutes 20/15 --> 15");
  t.is (quantizeToNMinutes (21, 15), 15, "quantizeToNMinutes 21/15 --> 15");
  t.is (quantizeToNMinutes (22, 15), 30, "quantizeToNMinutes 22/15 --> 30");
  t.is (quantizeToNMinutes (23, 15), 30, "quantizeToNMinutes 23/15 --> 30");
  t.is (quantizeToNMinutes (24, 15), 30, "quantizeToNMinutes 24/15 --> 30");
  t.is (quantizeToNMinutes (25, 15), 30, "quantizeToNMinutes 25/15 --> 30");
  t.is (quantizeToNMinutes (26, 15), 30, "quantizeToNMinutes 26/15 --> 30");
  t.is (quantizeToNMinutes (27, 15), 30, "quantizeToNMinutes 27/15 --> 30");
  t.is (quantizeToNMinutes (28, 15), 30, "quantizeToNMinutes 28/15 --> 30");
  t.is (quantizeToNMinutes (29, 15), 30, "quantizeToNMinutes 29/15 --> 30");
  t.is (quantizeToNMinutes (30, 15), 30, "quantizeToNMinutes 30/15 --> 30");
  t.is (quantizeToNMinutes (31, 15), 30, "quantizeToNMinutes 31/15 --> 30");
  t.is (quantizeToNMinutes (32, 15), 30, "quantizeToNMinutes 32/15 --> 30");
  t.is (quantizeToNMinutes (33, 15), 30, "quantizeToNMinutes 33/15 --> 30");
  t.is (quantizeToNMinutes (34, 15), 30, "quantizeToNMinutes 34/15 --> 30");
  t.is (quantizeToNMinutes (35, 15), 30, "quantizeToNMinutes 35/15 --> 30");
  t.is (quantizeToNMinutes (36, 15), 30, "quantizeToNMinutes 36/15 --> 30");
  t.is (quantizeToNMinutes (37, 15), 45, "quantizeToNMinutes 37/15 --> 45");
  t.is (quantizeToNMinutes (38, 15), 45, "quantizeToNMinutes 38/15 --> 45");
  t.is (quantizeToNMinutes (39, 15), 45, "quantizeToNMinutes 39/15 --> 45");
  t.is (quantizeToNMinutes (40, 15), 45, "quantizeToNMinutes 40/15 --> 45");
  t.is (quantizeToNMinutes (41, 15), 45, "quantizeToNMinutes 41/15 --> 45");
  t.is (quantizeToNMinutes (42, 15), 45, "quantizeToNMinutes 42/15 --> 45");
  t.is (quantizeToNMinutes (43, 15), 45, "quantizeToNMinutes 43/15 --> 45");
  t.is (quantizeToNMinutes (44, 15), 45, "quantizeToNMinutes 44/15 --> 45");
  t.is (quantizeToNMinutes (45, 15), 45, "quantizeToNMinutes 45/15 --> 45");
  t.is (quantizeToNMinutes (46, 15), 45, "quantizeToNMinutes 46/15 --> 45");
  t.is (quantizeToNMinutes (47, 15), 45, "quantizeToNMinutes 47/15 --> 45");
  t.is (quantizeToNMinutes (48, 15), 45, "quantizeToNMinutes 48/15 --> 45");
  t.is (quantizeToNMinutes (49, 15), 45, "quantizeToNMinutes 49/15 --> 45");
  t.is (quantizeToNMinutes (50, 15), 45, "quantizeToNMinutes 50/15 --> 45");
  t.is (quantizeToNMinutes (51, 15), 45, "quantizeToNMinutes 51/15 --> 45");
  t.is (quantizeToNMinutes (52, 15), 60, "quantizeToNMinutes 52/15 --> 60");
  t.is (quantizeToNMinutes (53, 15), 60, "quantizeToNMinutes 53/15 --> 60");
  t.is (quantizeToNMinutes (54, 15), 60, "quantizeToNMinutes 54/15 --> 60");
  t.is (quantizeToNMinutes (55, 15), 60, "quantizeToNMinutes 55/15 --> 60");
  t.is (quantizeToNMinutes (56, 15), 60, "quantizeToNMinutes 56/15 --> 60");
  t.is (quantizeToNMinutes (57, 15), 60, "quantizeToNMinutes 57/15 --> 60");
  t.is (quantizeToNMinutes (58, 15), 60, "quantizeToNMinutes 58/15 --> 60");
  t.is (quantizeToNMinutes (59, 15), 60, "quantizeToNMinutes 59/15 --> 60");
  t.is (quantizeToNMinutes (60, 15), 60, "quantizeToNMinutes 60/15 --> 60");

  t.is (quantizeToNMinutes (0, 20),   0, "quantizeToNMinutes 0/20 --> 0");
  t.is (quantizeToNMinutes (1, 20),   0, "quantizeToNMinutes 1/20 --> 0");
  t.is (quantizeToNMinutes (2, 20),   0, "quantizeToNMinutes 2/20 --> 0");
  t.is (quantizeToNMinutes (3, 20),   0, "quantizeToNMinutes 3/20 --> 0");
  t.is (quantizeToNMinutes (4, 20),   0, "quantizeToNMinutes 4/20 --> 0");
  t.is (quantizeToNMinutes (5, 20),   0, "quantizeToNMinutes 5/20 --> 0");
  t.is (quantizeToNMinutes (6, 20),   0, "quantizeToNMinutes 6/20 --> 0");
  t.is (quantizeToNMinutes (7, 20),   0, "quantizeToNMinutes 7/20 --> 0");
  t.is (quantizeToNMinutes (8, 20),   0, "quantizeToNMinutes 8/20 --> 0");
  t.is (quantizeToNMinutes (9, 20),   0, "quantizeToNMinutes 9/20 --> 0");
  t.is (quantizeToNMinutes (10, 20), 20, "quantizeToNMinutes 10/20 --> 20");
  t.is (quantizeToNMinutes (11, 20), 20, "quantizeToNMinutes 11/20 --> 20");
  t.is (quantizeToNMinutes (12, 20), 20, "quantizeToNMinutes 12/20 --> 20");
  t.is (quantizeToNMinutes (13, 20), 20, "quantizeToNMinutes 13/20 --> 20");
  t.is (quantizeToNMinutes (14, 20), 20, "quantizeToNMinutes 14/20 --> 20");
  t.is (quantizeToNMinutes (15, 20), 20, "quantizeToNMinutes 15/20 --> 20");
  t.is (quantizeToNMinutes (16, 20), 20, "quantizeToNMinutes 16/20 --> 20");
  t.is (quantizeToNMinutes (17, 20), 20, "quantizeToNMinutes 17/20 --> 20");
  t.is (quantizeToNMinutes (18, 20), 20, "quantizeToNMinutes 18/20 --> 20");
  t.is (quantizeToNMinutes (19, 20), 20, "quantizeToNMinutes 19/20 --> 20");
  t.is (quantizeToNMinutes (20, 20), 20, "quantizeToNMinutes 20/20 --> 20");
  t.is (quantizeToNMinutes (21, 20), 20, "quantizeToNMinutes 21/20 --> 20");
  t.is (quantizeToNMinutes (22, 20), 20, "quantizeToNMinutes 22/20 --> 20");
  t.is (quantizeToNMinutes (23, 20), 20, "quantizeToNMinutes 23/20 --> 20");
  t.is (quantizeToNMinutes (24, 20), 20, "quantizeToNMinutes 24/20 --> 20");
  t.is (quantizeToNMinutes (25, 20), 20, "quantizeToNMinutes 25/20 --> 20");
  t.is (quantizeToNMinutes (26, 20), 20, "quantizeToNMinutes 26/20 --> 20");
  t.is (quantizeToNMinutes (27, 20), 20, "quantizeToNMinutes 27/20 --> 20");
  t.is (quantizeToNMinutes (28, 20), 20, "quantizeToNMinutes 28/20 --> 20");
  t.is (quantizeToNMinutes (29, 20), 20, "quantizeToNMinutes 29/20 --> 20");
  t.is (quantizeToNMinutes (30, 20), 40, "quantizeToNMinutes 30/20 --> 40");
  t.is (quantizeToNMinutes (31, 20), 40, "quantizeToNMinutes 31/20 --> 40");
  t.is (quantizeToNMinutes (32, 20), 40, "quantizeToNMinutes 32/20 --> 40");
  t.is (quantizeToNMinutes (33, 20), 40, "quantizeToNMinutes 33/20 --> 40");
  t.is (quantizeToNMinutes (34, 20), 40, "quantizeToNMinutes 34/20 --> 40");
  t.is (quantizeToNMinutes (35, 20), 40, "quantizeToNMinutes 35/20 --> 40");
  t.is (quantizeToNMinutes (36, 20), 40, "quantizeToNMinutes 36/20 --> 40");
  t.is (quantizeToNMinutes (37, 20), 40, "quantizeToNMinutes 37/20 --> 40");
  t.is (quantizeToNMinutes (38, 20), 40, "quantizeToNMinutes 38/20 --> 40");
  t.is (quantizeToNMinutes (39, 20), 40, "quantizeToNMinutes 39/20 --> 40");
  t.is (quantizeToNMinutes (40, 20), 40, "quantizeToNMinutes 40/20 --> 40");
  t.is (quantizeToNMinutes (41, 20), 40, "quantizeToNMinutes 41/20 --> 40");
  t.is (quantizeToNMinutes (42, 20), 40, "quantizeToNMinutes 42/20 --> 40");
  t.is (quantizeToNMinutes (43, 20), 40, "quantizeToNMinutes 43/20 --> 40");
  t.is (quantizeToNMinutes (44, 20), 40, "quantizeToNMinutes 44/20 --> 40");
  t.is (quantizeToNMinutes (45, 20), 40, "quantizeToNMinutes 45/20 --> 40");
  t.is (quantizeToNMinutes (46, 20), 40, "quantizeToNMinutes 46/20 --> 40");
  t.is (quantizeToNMinutes (47, 20), 40, "quantizeToNMinutes 47/20 --> 40");
  t.is (quantizeToNMinutes (48, 20), 40, "quantizeToNMinutes 48/20 --> 40");
  t.is (quantizeToNMinutes (49, 20), 40, "quantizeToNMinutes 49/20 --> 40");
  t.is (quantizeToNMinutes (50, 20), 60, "quantizeToNMinutes 50/20 --> 60");
  t.is (quantizeToNMinutes (51, 20), 60, "quantizeToNMinutes 51/20 --> 60");
  t.is (quantizeToNMinutes (52, 20), 60, "quantizeToNMinutes 52/20 --> 60");
  t.is (quantizeToNMinutes (53, 20), 60, "quantizeToNMinutes 53/20 --> 60");
  t.is (quantizeToNMinutes (54, 20), 60, "quantizeToNMinutes 54/20 --> 60");
  t.is (quantizeToNMinutes (55, 20), 60, "quantizeToNMinutes 55/20 --> 60");
  t.is (quantizeToNMinutes (56, 20), 60, "quantizeToNMinutes 56/20 --> 60");
  t.is (quantizeToNMinutes (57, 20), 60, "quantizeToNMinutes 57/20 --> 60");
  t.is (quantizeToNMinutes (58, 20), 60, "quantizeToNMinutes 58/20 --> 60");
  t.is (quantizeToNMinutes (59, 20), 60, "quantizeToNMinutes 59/20 --> 60");
  t.is (quantizeToNMinutes (60, 20), 60, "quantizeToNMinutes 60/20 --> 60");

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

