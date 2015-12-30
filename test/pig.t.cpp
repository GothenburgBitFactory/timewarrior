////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015, Paul Beckingham, Federico Hernandez.
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
#include <Pig.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (25);

  // Pig::skipN
  Pig p0 ("12345");
  t.ok (p0.skipN (4),       "skipN=4 '12345' -> true");
  t.is (p0.dump (),         "≪12345≫ l5 c4", "dump");
  t.diag (p0.dump ());

  t.notok (p0.skipN (2),    "skipN=2 '5' -> false");
  t.is (p0.dump (),         "≪12345≫ l5 c4", "dump");
  t.diag (p0.dump ());

  // Pig::skipWS
  Pig p1 ("  one");
  t.ok (p1.skipWS (),       "skipWS '  one' -> true");
  t.is (p1.dump (),         "≪  one≫ l5 c2", "dump");
  t.diag (p1.dump ());

  t.notok (p1.skipWS (),    "skipWS 'one' -> false");
  t.is (p1.dump (),         "≪  one≫ l5 c2", "dump");
  t.diag (p1.dump ());

  // Pig::skipLiteral
  Pig p2 ("onetwo");
  t.notok (p2.skipLiteral ("two"), "skipLiteral=two 'onetwo' -> false");
  t.ok (p2.skipLiteral ("one"),    "skipLiteral=one 'onetwo' -> true");
  t.is (p2.dump (),                "≪onetwo≫ l6 c3", "dump");
  t.diag (p2.dump ());

  // Pig::getDigit
  Pig p3 (" 123");
  int n;
  t.notok (p3.getDigit (n), "getDigit ' 123' --> false");
  t.ok (p3.skipWS (),       "skipWS ' 123' --> true");
  t.is (p3.dump (),         "≪ 123≫ l4 c1", "dump");
  t.diag (p3.dump ());
  t.ok (p3.getDigit (n),    "getDigit '123' --> true");
  t.is (n, 1,               "getDigit '123' --> '1'");
  t.is (p3.dump (),         "≪ 123≫ l4 c2", "dump");
  t.diag (p3.dump ());

  // Pig::getDigits
  Pig p4 ("123 ");
  t.ok (p4.getDigits (n),   "getDigits '123 ' --> true");
  t.is (n, 123,             "getDigits '123 ' --> 123");
  t.is (p4.dump (),         "≪123 ≫ l4 c3", "dump");
  t.diag (p4.dump ());

  Pig p5 ("1");
  t.notok (p5.eos (),       "eos '1' --> false");
  t.ok (p5.getDigit (n),    "getDigit '1' --> true");
  t.notok (p5.getDigit (n), "getDigit '' --> false");
  t.ok (p5.eos (),          "eos '' --> true");
  t.is (p5.dump (),         "≪1≫ l1 c1", "dump");
  t.diag (p5.dump ());

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
