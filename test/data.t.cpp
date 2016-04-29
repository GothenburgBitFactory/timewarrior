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
  UnitTest t (15);

  // std::vector <Interval> splitInterval (const Interval&, std::vector <Range>&);
  Interval i1;
  i1.range = Range (Datetime ("20160427T000000Z"), Datetime ("20160428T000000Z"));
  i1.tag ("foo");

  std::vector <Range> exclusions = {Range (Datetime ("20160427T000000Z"), Datetime ("20160427T080000Z")),
                                    Range (Datetime ("20160427T120000Z"), Datetime ("20160427T130000Z")),
                                    Range (Datetime ("20160427T173000Z"), Datetime ("20160428T000000Z"))};

  auto results = collapse (i1, exclusions);
  t.ok (results.size () == 2,                                "splitInterval --> 2 fragments");
  t.is (results[0].range.start.toISO (), "20160427T080000Z", "splitInterval --> results[0].range.start 20160427T080000Z");
  t.is (results[0].range.end.toISO (),   "20160427T120000Z", "splitInterval --> results[0].range.end   20160427T120000Z");
  t.is (results[1].range.start.toISO (), "20160427T130000Z", "splitInterval --> results[1].range.start 20160427T130000Z");
  t.is (results[1].range.end.toISO (),   "20160427T173000Z", "splitInterval --> results[1].range.end   20160427T173000Z");

  Interval i2;
  i2.range = Range (Datetime ("20160427T115500Z"), Datetime ("20160427T130500Z"));
  i2.tag ("foo");

  results = collapse (i2, exclusions);
  t.ok (results.size () == 2,                                "splitInterval --> 2 fragments");
  t.is (results[0].range.start.toISO (), "20160427T115500Z", "splitInterval --> results[0].range.start 20160427T115500Z");
  t.is (results[0].range.end.toISO (),   "20160427T120000Z", "splitInterval --> results[0].range.end   20160427T120000Z");
  t.is (results[1].range.start.toISO (), "20160427T130000Z", "splitInterval --> results[1].range.start 20160427T130000Z");
  t.is (results[1].range.end.toISO (),   "20160427T130500Z", "splitInterval --> results[1].range.end   20160427T130500Z");

  Interval i3;
  i3.range = Range (Datetime ("20160427T160000Z"), Datetime (0));
  i3.tag ("foo");
  t.ok    (i3.range.started (), "i3 range started");
  t.notok (i3.range.ended (),   "i3 range not ended");

  results = collapse (i3, exclusions);
  t.ok (results.size () == 2,                                "splitInterval --> 2 fragments");
  t.is (results[0].range.start.toISO (), "20160427T160000Z", "splitInterval --> results[0].range.start 20160427T160000Z");
  t.is (results[0].range.end.toISO (),   "20160427T173000Z", "splitInterval --> results[0].range.end   20160427T173000Z");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

