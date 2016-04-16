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
  UnitTest t (21);

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

  // this                     |--------|
  // other      |--------|                                   [A] false
  // other               |--------|                          [B] true
  // other                      |----|                       [C] true
  // other                         |--------|                [D] true
  // other                                  |--------|       [E] false
  // other                  |-------------|                  [F] true
  // other                  |...                             [G] true
  // other                       |...                        [H] true
  // other                               |...                [I] false
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

  // this                     |...
  // other      |--------|                                   [A] false
  // other               |--------|                          [B] true
  // other                      |----|                       [C] true
  // other                  |...                             [G] true
  // other                       |...                        [H] true
  // other                               |...                [I] true
  Daterange refOpen;
  refOpen.start (Datetime (6, 1, 2016));

  t.notok (refOpen.overlap (testA), "Daterange: ! refOpen.overlap(testA)");
  t.ok    (refOpen.overlap (testB), "Daterange:   refOpen.overlap(testB)");
  t.ok    (refOpen.overlap (testC), "Daterange:   refOpen.overlap(testC)");
  t.ok    (refOpen.overlap (testG), "Daterange:   refOpen.overlap(testG)");
  t.ok    (refOpen.overlap (testH), "Daterange:   refOpen.overlap(testH)");
  t.ok    (refOpen.overlap (testI), "Daterange: ! refOpen.overlap(testI)");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

