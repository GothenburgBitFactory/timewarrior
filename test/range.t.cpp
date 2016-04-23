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
#include <Range.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (60);

  // bool isStarted () const;
  // bool isEnded () const;
  Range i1;
  t.is (i1.isStarted (), false, "Range().isStarted -> false");
  t.is (i1.isEnded (),   false, "Range().isEnded -> false");

  // void start (Datetime);
  i1.start (Datetime ());
  t.is (i1.isStarted (), true,  "Range(start=now).isStarted -> true");
  t.is (i1.isEnded (),   false, "Range(start=now).isEnded -> false");

  // void end (Datetime);
  i1.end (Datetime ());
  t.is (i1.isStarted (), true, "Range(start=now,end=now).isStarted -> true");
  t.is (i1.isEnded (),   true, "Range(start=now,end=now).isEnded -> true");

  // this                     [--------)
  // A          [--------)
  // B                   [--------)
  // C                          [----|
  // D                             [--------)
  // E                                      [--------)
  // F                      [-------------)
  // G                      [...
  // H                           [...
  // I                                   [...
  Range refClosed;
  refClosed.start (Datetime (6,  1, 2016));
  refClosed.end   (Datetime (6, 30, 2016));

  Range testA; testA.start (Datetime (4,  1, 2016)); testA.end (Datetime (4, 30, 2016));
  Range testB; testB.start (Datetime (5, 15, 2016)); testB.end (Datetime (6, 15, 2016));
  Range testC; testC.start (Datetime (6, 10, 2016)); testC.end (Datetime (6, 20, 2016));
  Range testD; testD.start (Datetime (6, 15, 2016)); testD.end (Datetime (7, 15, 2016));
  Range testE; testE.start (Datetime (8,  1, 2016)); testE.end (Datetime (8, 31, 2016));
  Range testF; testF.start (Datetime (5, 15, 2016)); testF.end (Datetime (7, 15, 2016));
  Range testG; testG.start (Datetime (5, 15, 2016));
  Range testH; testH.start (Datetime (6, 15, 2016));
  Range testI; testI.start (Datetime (7, 15, 2016));

  t.notok (refClosed.overlap (testA), "Range: ! refClosed.overlap(testA)");
  t.ok    (refClosed.overlap (testB), "Range:   refClosed.overlap(testB)");
  t.ok    (refClosed.overlap (testC), "Range:   refClosed.overlap(testC)");
  t.ok    (refClosed.overlap (testD), "Range:   refClosed.overlap(testD)");
  t.notok (refClosed.overlap (testE), "Range: ! refClosed.overlap(testE)");
  t.ok    (refClosed.overlap (testF), "Range:   refClosed.overlap(testF)");
  t.ok    (refClosed.overlap (testG), "Range:   refClosed.overlap(testG)");
  t.ok    (refClosed.overlap (testH), "Range:   refClosed.overlap(testH)");
  t.notok (refClosed.overlap (testI), "Range: ! refClosed.overlap(testI)");

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
  Range refOpen;
  refOpen.start (Datetime (6, 1, 2016));

  t.notok (refOpen.overlap (testA), "Range: ! refOpen.overlap(testA)");
  t.ok    (refOpen.overlap (testB), "Range:   refOpen.overlap(testB)");
  t.ok    (refOpen.overlap (testC), "Range:   refOpen.overlap(testC)");
  t.ok    (refOpen.overlap (testD), "Range:   refOpen.overlap(testD)");
  t.ok    (refOpen.overlap (testE), "Range:   refOpen.overlap(testE)");
  t.ok    (refOpen.overlap (testF), "Range:   refOpen.overlap(testF)");
  t.ok    (refOpen.overlap (testG), "Range:   refOpen.overlap(testG)");
  t.ok    (refOpen.overlap (testH), "Range:   refOpen.overlap(testH)");
  t.ok    (refOpen.overlap (testI), "Range:   refOpen.overlap(testI)");

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
  Range empty;
  t.ok (refClosed.intersect (testA) == empty,                                    "Range: refClosed.intersect(testA) == empty");
  t.ok (refClosed.intersect (testB) == Range (refClosed.start (), testB.end ()), "Range: refClosed.intersect(testB) == Range(refClosed.start(), testB.end())");
  t.ok (refClosed.intersect (testC) == testC,                                    "Range: refClosed.intersect(testB) == testC");
  t.ok (refClosed.intersect (testD) == Range (testD.start (), refClosed.end ()), "Range: refClosed.intersect(testB) == Range(testD.start(), refClosed.end())");
  t.ok (refClosed.intersect (testE) == empty,                                    "Range: refClosed.intersect(testE) == empty");
  t.ok (refClosed.intersect (testF) == refClosed,                                "Range: refClosed.intersect(testF) == refClosed");
  t.ok (refClosed.intersect (testG) == refClosed,                                "Range: refClosed.intersect(testG) == refClosed");
  t.ok (refClosed.intersect (testH) == Range (testH.start (), refClosed.end ()), "Range: refClosed.intersect(testH) == Range(testH.start(), refClosed.end())");
  t.ok (refClosed.intersect (testI) == empty,                                    "Range: refClosed.intersect(testI) == empty");

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
  t.ok (refOpen.intersect (testA) == empty,                                  "Range: refOpen.intersect(testA) == empty");
  t.ok (refOpen.intersect (testB) == Range (refOpen.start (), testB.end ()), "Range: refOpen.intersect(testB) == Range(refOpen.start(), testB.end())");
  t.ok (refOpen.intersect (testC) == testC,                                  "Range: refOpen.intersect(testC) == testC");
  t.ok (refOpen.intersect (testD) == testD,                                  "Range: refOpen.intersect(testD) == testD");
  t.ok (refOpen.intersect (testE) == testE,                                  "Range: refOpen.intersect(testE) == testE");
  t.ok (refOpen.intersect (testF) == Range (refOpen.start (), testF.end ()), "Range: refOpen.intersect(testF) == Range(refOpen.start(), testF.end()");
  t.ok (refOpen.intersect (testG) == refOpen,                                "Range: refOpen.intersect(testG) == refOpen");
  t.ok (refOpen.intersect (testH) == testH,                                  "Range: refOpen.intersect(testH) == testH");
  t.ok (refOpen.intersect (testI) == testI,                                  "Range: refOpen.intersect(testI) == testI");

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
  std::vector <Range> closedSubtractA {refClosed};
  std::vector <Range> closedSubtractB {Range (testB.end (), refClosed.end ())};
  std::vector <Range> closedSubtractC {Range (refClosed.start (), testC.start ()), Range (testC.end (), refClosed.end ())};
  std::vector <Range> closedSubtractD {Range (refClosed.start (), testD.start ())};
  std::vector <Range> closedSubtractE {refClosed};
  std::vector <Range> closedSubtractF {};
  std::vector <Range> closedSubtractG {};
  std::vector <Range> closedSubtractH {Range (refClosed.start (), testH.start ())};
  std::vector <Range> closedSubtractI {refClosed};

  t.ok (refClosed.subtract (testA) == closedSubtractA, "Range: refClosed.subtract(testA) == {refClosed}");
  t.ok (refClosed.subtract (testB) == closedSubtractB, "Range: refClosed.subtract(testB) == {Range(testB.end(), refClosed.end())}");
  t.ok (refClosed.subtract (testC) == closedSubtractC, "Range: refClosed.subtract(testC) == {Range(refClosed.start(), testC.start()),"
                                                                                            "Range(testC.end(), refClosed.end()}");
  t.ok (refClosed.subtract (testD) == closedSubtractD, "Range: refClosed.subtract(testD) == {Range(refClosed.start(), testD.start())}");
  t.ok (refClosed.subtract (testE) == closedSubtractE, "Range: refClosed.subtract(testE) == {refClosed}");
  t.ok (refClosed.subtract (testF) == closedSubtractF, "Range: refClosed.subtract(testF) == {}");
  t.ok (refClosed.subtract (testG) == closedSubtractG, "Range: refClosed.subtract(testG) == {}");
  t.ok (refClosed.subtract (testH) == closedSubtractH, "Range: refClosed.subtract(testH) == {refClosed.start(), testH.start()}");
  t.ok (refClosed.subtract (testI) == closedSubtractI, "Range: refClosed.subtract(testI) == {refClosed}");

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
  std::vector <Range> openSubtractA {refOpen};
  std::vector <Range> openSubtractB {Range (testB.end (), refOpen.end ())};
  std::vector <Range> openSubtractC {Range (refOpen.start (), testC.start ()), Range (testC.end (), refOpen.end ())};
  std::vector <Range> openSubtractD {Range (refOpen.start (), testD.start ()), Range (testD.end (), refOpen.end ())};
  std::vector <Range> openSubtractE {Range (refOpen.start (), testE.start ()), Range (testE.end (), refOpen.end ())};
  std::vector <Range> openSubtractF {Range (testF.end (), refOpen.end ())};
  std::vector <Range> openSubtractG {};
  std::vector <Range> openSubtractH {Range (refOpen.start (), testH.start ())};
  std::vector <Range> openSubtractI {Range (refOpen.start (), testI.start ())};

  t.ok (refOpen.subtract (testA) == openSubtractA, "Range: refOpen.subtract(testA) == {refOpen}");
  t.ok (refOpen.subtract (testB) == openSubtractB, "Range: refOpen.subtract(testB) == {Range(testB.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testC) == openSubtractC, "Range: refOpen.subtract(testC) == {Range(refOpen.start(), testC.start()),"
                                                                                      "Range(testC.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testD) == openSubtractD, "Range: refOpen.subtract(testD) == {Range(refOpen.start(), testD.start()),"
                                                                                      "Range(testD.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testE) == openSubtractE, "Range: refOpen.subtract(testE) == {Range(refOpen.start(), testE.start()),"
                                                                                      "Range(testE.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testF) == openSubtractF, "Range: refOpen.subtract(testF) == {Range(testF.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testG) == openSubtractG, "Range: refOpen.subtract(testG) == {}");
  t.ok (refOpen.subtract (testH) == openSubtractH, "Range: refOpen.subtract(testH) == {Range(refOpen.start(), testH.start()}");
  t.ok (refOpen.subtract (testI) == openSubtractI, "Range: refOpen.subtract(testI) == {Range(refOpen.start(), testI.start()}");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

