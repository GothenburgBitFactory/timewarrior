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
  UnitTest t (7);


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

