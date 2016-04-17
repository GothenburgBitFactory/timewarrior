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
#include <Daterange.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (60);

  // bool isStarted () const;
  // bool isEnded () const;
  Daterange i1;
  t.is (i1.isStarted (), false, "Daterange().isStarted -> false");
  t.is (i1.isEnded (),   false, "Daterange().isEnded -> false");

  // void start (Datetime);
  i1.start (Datetime ());
  t.is (i1.isStarted (), true,  "Daterange(start=now).isStarted -> true");
  t.is (i1.isEnded (),   false, "Daterange(start=now).isEnded -> false");

  // void end (Datetime);
  i1.end (Datetime ());
  t.is (i1.isStarted (), true, "Daterange(start=now,end=now).isStarted -> true");
  t.is (i1.isEnded (),   true, "Daterange(start=now,end=now).isEnded -> true");

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
  Daterange refClosed;
  refClosed.start (Datetime (6,  1, 2016));
  refClosed.end   (Datetime (6, 30, 2016));

  Daterange testA; testA.start (Datetime (4,  1, 2016)); testA.end (Datetime (4, 30, 2016));
  Daterange testB; testB.start (Datetime (5, 15, 2016)); testB.end (Datetime (6, 15, 2016));
  Daterange testC; testC.start (Datetime (6, 10, 2016)); testC.end (Datetime (6, 20, 2016));
  Daterange testD; testD.start (Datetime (6, 15, 2016)); testD.end (Datetime (7, 15, 2016));
  Daterange testE; testE.start (Datetime (8,  1, 2016)); testE.end (Datetime (8, 31, 2016));
  Daterange testF; testF.start (Datetime (5, 15, 2016)); testF.end (Datetime (7, 15, 2016));
  Daterange testG; testG.start (Datetime (5, 15, 2016));
  Daterange testH; testH.start (Datetime (6, 15, 2016));
  Daterange testI; testI.start (Datetime (7, 15, 2016));

  t.notok (refClosed.overlap (testA), "Daterange: ! refClosed.overlap(testA)");
  t.ok    (refClosed.overlap (testB), "Daterange:   refClosed.overlap(testB)");
  t.ok    (refClosed.overlap (testC), "Daterange:   refClosed.overlap(testC)");
  t.ok    (refClosed.overlap (testD), "Daterange:   refClosed.overlap(testD)");
  t.notok (refClosed.overlap (testE), "Daterange: ! refClosed.overlap(testE)");
  t.ok    (refClosed.overlap (testF), "Daterange:   refClosed.overlap(testF)");
  t.ok    (refClosed.overlap (testG), "Daterange:   refClosed.overlap(testG)");
  t.ok    (refClosed.overlap (testH), "Daterange:   refClosed.overlap(testH)");
  t.notok (refClosed.overlap (testI), "Daterange: ! refClosed.overlap(testI)");

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
  Daterange refOpen;
  refOpen.start (Datetime (6, 1, 2016));

  t.notok (refOpen.overlap (testA), "Daterange: ! refOpen.overlap(testA)");
  t.ok    (refOpen.overlap (testB), "Daterange:   refOpen.overlap(testB)");
  t.ok    (refOpen.overlap (testC), "Daterange:   refOpen.overlap(testC)");
  t.ok    (refOpen.overlap (testD), "Daterange:   refOpen.overlap(testD)");
  t.ok    (refOpen.overlap (testE), "Daterange:   refOpen.overlap(testE)");
  t.ok    (refOpen.overlap (testF), "Daterange:   refOpen.overlap(testF)");
  t.ok    (refOpen.overlap (testG), "Daterange:   refOpen.overlap(testG)");
  t.ok    (refOpen.overlap (testH), "Daterange:   refOpen.overlap(testH)");
  t.ok    (refOpen.overlap (testI), "Daterange:   refOpen.overlap(testI)");

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
  Daterange empty;
  t.ok (refClosed.intersect (testA) == empty,                                         "Daterange: refClosed.intersect(testA) == empty");
  t.ok (refClosed.intersect (testB) == Daterange (refClosed.start (), testB.end ()),  "Daterange: refClosed.intersect(testB) == Daterange(refClosed.start(), testB.end())");
  t.ok (refClosed.intersect (testC) == testC,                                         "Daterange: refClosed.intersect(testB) == testC");
  t.ok (refClosed.intersect (testD) == Daterange (testD.start (), refClosed.end ()),  "Daterange: refClosed.intersect(testB) == Daterange(testD.start(), refClosed.end())");
  t.ok (refClosed.intersect (testE) == empty,                                         "Daterange: refClosed.intersect(testE) == empty");
  t.ok (refClosed.intersect (testF) == refClosed,                                     "Daterange: refClosed.intersect(testF) == refClosed");
  t.ok (refClosed.intersect (testG) == refClosed,                                     "Daterange: refClosed.intersect(testG) == refClosed");
  t.ok (refClosed.intersect (testH) == Daterange (testH.start (), refClosed.end ()),  "Daterange: refClosed.intersect(testH) == Daterange(testH.start(), refClosed.end())");
  t.ok (refClosed.intersect (testI) == empty,                                         "Daterange: refClosed.intersect(testI) == empty");

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
  t.ok (refOpen.intersect (testA) == empty,                                           "Daterange: refOpen.intersect(testA) == empty");
  t.ok (refOpen.intersect (testB) == Daterange (refOpen.start (), testB.end ()),      "Daterange: refOpen.intersect(testB) == Daterange(refOpen.start(), testB.end())");
  t.ok (refOpen.intersect (testC) == testC,                                           "Daterange: refOpen.intersect(testC) == testC");
  t.ok (refOpen.intersect (testD) == testD,                                           "Daterange: refOpen.intersect(testD) == testD");
  t.ok (refOpen.intersect (testE) == testE,                                           "Daterange: refOpen.intersect(testE) == testE");
  t.ok (refOpen.intersect (testF) == Daterange (refOpen.start (), testF.end ()),      "Daterange: refOpen.intersect(testF) == Daterange(refOpen.start(), testF.end()");
  t.ok (refOpen.intersect (testG) == refOpen,                                         "Daterange: refOpen.intersect(testG) == refOpen");
  t.ok (refOpen.intersect (testH) == testH,                                           "Daterange: refOpen.intersect(testH) == testH");
  t.ok (refOpen.intersect (testI) == testI,                                           "Daterange: refOpen.intersect(testI) == testI");

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
  std::vector <Daterange> closedSubtractA {refClosed};
  std::vector <Daterange> closedSubtractB {Daterange (testB.end (), refClosed.end ())};
  std::vector <Daterange> closedSubtractC {Daterange (refClosed.start (), testC.start ()), Daterange (testC.end (), refClosed.end ())};
  std::vector <Daterange> closedSubtractD {Daterange (refClosed.start (), testD.start ())};
  std::vector <Daterange> closedSubtractE {refClosed};
  std::vector <Daterange> closedSubtractF {};
  std::vector <Daterange> closedSubtractG {};
  std::vector <Daterange> closedSubtractH {Daterange (refClosed.start (), testH.start ())};
  std::vector <Daterange> closedSubtractI {refClosed};

  t.ok (refClosed.subtract (testA) == closedSubtractA, "Daterange: refClosed.subtract(testA) == {refClosed}");
  t.ok (refClosed.subtract (testB) == closedSubtractB, "Daterange: refClosed.subtract(testB) == {Daterange(testB.end(), refClosed.end())}");
  t.ok (refClosed.subtract (testC) == closedSubtractC, "Daterange: refClosed.subtract(testC) == {Daterange(refClosed.start(), testC.start()),"
                                                                                               "Daterange(testC.end(), refClosed.end()}");
  t.ok (refClosed.subtract (testD) == closedSubtractD, "Daterange: refClosed.subtract(testD) == {Daterange(refClosed.start(), testD.start())}");
  t.ok (refClosed.subtract (testE) == closedSubtractE, "Daterange: refClosed.subtract(testE) == {refClosed}");
  t.ok (refClosed.subtract (testF) == closedSubtractF, "Daterange: refClosed.subtract(testF) == {}");
  t.ok (refClosed.subtract (testG) == closedSubtractG, "Daterange: refClosed.subtract(testG) == {}");
  t.ok (refClosed.subtract (testH) == closedSubtractH, "Daterange: refClosed.subtract(testH) == {refClosed.start(), testH.start()}");
  t.ok (refClosed.subtract (testI) == closedSubtractI, "Daterange: refClosed.subtract(testI) == {refClosed}");

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
  std::vector <Daterange> openSubtractA {refOpen};
  std::vector <Daterange> openSubtractB {Daterange (testB.end (), refOpen.end ())};
  std::vector <Daterange> openSubtractC {Daterange (refOpen.start (), testC.start ()), Daterange (testC.end (), refOpen.end ())};
  std::vector <Daterange> openSubtractD {Daterange (refOpen.start (), testD.start ()), Daterange (testD.end (), refOpen.end ())};
  std::vector <Daterange> openSubtractE {Daterange (refOpen.start (), testE.start ()), Daterange (testE.end (), refOpen.end ())};
  std::vector <Daterange> openSubtractF {Daterange (testF.end (), refOpen.end ())};
  std::vector <Daterange> openSubtractG {};
  std::vector <Daterange> openSubtractH {Daterange (refOpen.start (), testH.start ())};
  std::vector <Daterange> openSubtractI {Daterange (refOpen.start (), testI.start ())};

  t.ok (refOpen.subtract (testA) == openSubtractA, "Daterange: refOpen.subtract(testA) == {refOpen}");
  t.ok (refOpen.subtract (testB) == openSubtractB, "Daterange: refOpen.subtract(testB) == {Daterange(testB.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testC) == openSubtractC, "Daterange: refOpen.subtract(testC) == {Daterange(refOpen.start(), testC.start()),"
                                                                                          "Daterange(testC.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testD) == openSubtractD, "Daterange: refOpen.subtract(testD) == {Daterange(refOpen.start(), testD.start()),"
                                                                                          "Daterange(testD.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testE) == openSubtractE, "Daterange: refOpen.subtract(testE) == {Daterange(refOpen.start(), testE.start()),"
                                                                                          "Daterange(testE.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testF) == openSubtractF, "Daterange: refOpen.subtract(testF) == {Daterange(testF.end(), refOpen.end()}");
  t.ok (refOpen.subtract (testG) == openSubtractG, "Daterange: refOpen.subtract(testG) == {}");
  t.ok (refOpen.subtract (testH) == openSubtractH, "Daterange: refOpen.subtract(testH) == {Daterange(refOpen.start(), testH.start()}");
  t.ok (refOpen.subtract (testI) == openSubtractI, "Daterange: refOpen.subtract(testI) == {Daterange(refOpen.start(), testI.start()}");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

