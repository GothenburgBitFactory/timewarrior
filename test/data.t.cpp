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
  UnitTest t (36);

  // std::vector <Interval> collapse (const Interval&, std::vector <Range>&);
  Interval i1;
  i1.range = Range (Datetime ("20160427T000000Z"), Datetime ("20160428T000000Z"));
  i1.tag ("foo");

  std::vector <Range> exclusions = {Range (Datetime ("20160427T000000Z"), Datetime ("20160427T080000Z")),
                                    Range (Datetime ("20160427T120000Z"), Datetime ("20160427T130000Z")),
                                    Range (Datetime ("20160427T173000Z"), Datetime ("20160428T000000Z"))};

  auto results = collapse (i1, exclusions);
  t.ok (results.size () == 2,                                "collapse i1 --> 2 fragments");
  t.is (results[0].range.start.toISO (), "20160427T000000Z", "collapse i1 --> results[0].range.start 20160427T000000Z");
  t.is (results[0].range.end.toISO (),   "20160427T120000Z", "collapse i1 --> results[0].range.end   20160427T120000Z");
  t.is (results[1].range.start.toISO (), "20160427T130000Z", "collapse i1 --> results[1].range.start 20160427T130000Z");
  t.is (results[1].range.end.toISO (),   "20160428T000000Z", "collapse i1 --> results[1].range.end   20160428T000000Z");

  Interval i2;
  i2.range = Range (Datetime ("20160427T115500Z"), Datetime ("20160427T130500Z"));
  i2.tag ("foo");

  results = collapse (i2, exclusions);
  t.ok (results.size () == 2,                                "collapse i2 --> 2 fragments");
  t.is (results[0].range.start.toISO (), "20160427T115500Z", "collapse i2 --> results[0].range.start 20160427T115500Z");
  t.is (results[0].range.end.toISO (),   "20160427T120000Z", "collapse i2 --> results[0].range.end   20160427T120000Z");
  t.is (results[1].range.start.toISO (), "20160427T130000Z", "collapse i2 --> results[1].range.start 20160427T130000Z");
  t.is (results[1].range.end.toISO (),   "20160427T130500Z", "collapse i2 --> results[1].range.end   20160427T130500Z");

  Interval i3;
  i3.range = Range (Datetime ("20160427T160000Z"), Datetime (0));
  i3.tag ("foo");
  t.ok    (i3.range.started (), "i3 range started");
  t.notok (i3.range.ended (),   "i3 range not ended");

  results = collapse (i3, exclusions);
  t.ok (results.size () == 2,                                "collapse i3 --> 2 fragments");
  t.is (results[0].range.start.toISO (), "20160427T160000Z", "collapse i3 --> results[0].range.start 20160427T160000Z");
  t.is (results[0].range.end.toISO (),   "20160427T173000Z", "collapse i3 --> results[0].range.end   20160427T173000Z");

  // bool matchesFilter (const Interval& interval, const Interval& filter);
  Interval refOpen;
  refOpen.range = Range (Datetime (2016, 6, 1), Datetime (0));
  refOpen.tag ("tag1");
  refOpen.tag ("tag2");

  Interval refClosed;
  refClosed.range = Range (Datetime (2016, 6, 1), Datetime (2016, 6, 30));
  refClosed.tag ("tag1");
  refClosed.tag ("tag2");

  t.ok (matchesFilter (refOpen, refClosed), "matchesFilter 2016-06-01- tag1 tag2 <=> 2016-06-01-2016-06-30 tag1 tag2");

  // this                     [--------)
  // A          [--------)
  // B                   [--------)
  // C                          [----)
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  Range testA; testA.start = Datetime (2016, 4,  1); testA.end = Datetime (2016, 4, 30);
  Range testB; testB.start = Datetime (2016, 5, 15); testB.end = Datetime (2016, 6, 15);
  Range testC; testC.start = Datetime (2016, 6, 10); testC.end = Datetime (2016, 6, 20);
  Range testD; testD.start = Datetime (2016, 6, 15); testD.end = Datetime (2016, 7, 15);
  Range testE; testE.start = Datetime (2016, 8,  1); testE.end = Datetime (2016, 8, 31);
  Range testF; testF.start = Datetime (2016, 5, 15); testF.end = Datetime (2016, 7, 15);
  Range testG; testG.start = Datetime (2016, 5, 15);
  Range testH; testH.start = Datetime (2016, 6, 15);
  Range testI; testI.start = Datetime (2016, 7, 15);

  Interval i;
  i.tag ("tag1");
  i.tag ("tag2");
  i.range = testA; t.notok (matchesFilter (i, refClosed), "matchesFilter A <!> refClosed");
  i.range = testB; t.ok    (matchesFilter (i, refClosed), "matchesFilter B <=> refClosed");
  i.range = testC; t.ok    (matchesFilter (i, refClosed), "matchesFilter C <=> refClosed");
  i.range = testD; t.ok    (matchesFilter (i, refClosed), "matchesFilter D <=> refClosed");
  i.range = testE; t.notok (matchesFilter (i, refClosed), "matchesFilter E <!> refClosed");
  i.range = testF; t.ok    (matchesFilter (i, refClosed), "matchesFilter F <=> refClosed");
  i.range = testG; t.ok    (matchesFilter (i, refClosed), "matchesFilter G <=> refClosed");
  i.range = testH; t.ok    (matchesFilter (i, refClosed), "matchesFilter H <=> refClosed");
  i.range = testI; t.notok (matchesFilter (i, refClosed), "matchesFilter I <!> refClosed");

  // this                     [...
  // A          [--------)
  // B                   [--------)
  // C                          [----)
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  i.range = testA; t.notok (matchesFilter (i, refOpen), "matchesFilter A <!> refOpen");
  i.range = testB; t.ok    (matchesFilter (i, refOpen), "matchesFilter B <=> refOpen");
  i.range = testC; t.ok    (matchesFilter (i, refOpen), "matchesFilter C <=> refOpen");
  i.range = testD; t.ok    (matchesFilter (i, refOpen), "matchesFilter D <=> refOpen");
  i.range = testE; t.ok    (matchesFilter (i, refOpen), "matchesFilter E <=> refOpen");
  i.range = testF; t.ok    (matchesFilter (i, refOpen), "matchesFilter F <=> refOpen");
  i.range = testG; t.ok    (matchesFilter (i, refOpen), "matchesFilter G <=> refOpen");
  i.range = testH; t.ok    (matchesFilter (i, refOpen), "matchesFilter H <=> refOpen");
  i.range = testI; t.ok    (matchesFilter (i, refOpen), "matchesFilter I <=> refOpen");

  // Range getFullDay (const Datetime&);
  auto r1 = getFullDay (Datetime (2016, 5, 1, 20, 31, 12));
  t.ok (r1.start == Datetime (2016, 5, 1,  0,  0,  0), "getFullDay 2016-05-01T20:31:23 -> start 2016-05-01T00:00:00");
  t.ok (r1.end   == Datetime (2016, 5, 1, 23, 59, 59), "getFullDay 2016-05-01T20:31:23 -> end   2016-05-01T23:59:59");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

