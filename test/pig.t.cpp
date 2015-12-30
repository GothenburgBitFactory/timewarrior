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
  UnitTest t (65);

  // Pig::skipN
  Pig p0 ("12345");
  t.ok (p0.skipN (4),       "skipN=4 '12345' -> true");
  t.is (p0.dump (),         "≪12345≫ l5 c4", "dump: " + p0.dump ());

  t.notok (p0.skipN (2),    "skipN=2 '5' -> false");
  t.is (p0.dump (),         "≪12345≫ l5 c4", "dump: " + p0.dump ());

  // Pig::skipWS
  Pig p1 ("  one");
  t.ok (p1.skipWS (),       "skipWS '  one' -> true");
  t.is (p1.dump (),         "≪  one≫ l5 c2", "dump: " + p1.dump ());

  t.notok (p1.skipWS (),    "skipWS 'one' -> false");
  t.is (p1.dump (),         "≪  one≫ l5 c2", "dump: " + p1.dump ());

  // Pig::skipLiteral
  Pig p2 ("onetwo");
  t.notok (p2.skipLiteral ("two"), "skipLiteral=two 'onetwo' -> false");
  t.ok (p2.skipLiteral ("one"),    "skipLiteral=one 'onetwo' -> true");
  t.is (p2.dump (),                "≪onetwo≫ l6 c3", "dump: " + p2.dump ());

  // Pig::getUntilWS
  Pig p3 ("one two three");
  std::string value;
  t.ok (p3.getUntilWS (value), "getUntilWS 'one two three' --> true");
  t.is (value, "one",          "getUntilWS 'one two three' --> 'one'");
  t.is (p3.dump (),            "≪one two three≫ l13 c3", "dump: " + p3.dump ());

  t.ok (p3.skipWS (),          "skipWS ' two three' --> true"); 

  t.ok (p3.getUntilWS (value), "getUntilWS 'two three' --> true");
  t.is (value, "two",          "getUntilWS 'two three' --> 'two'");
  t.is (p3.dump (),            "≪one two three≫ l13 c7", "dump: " + p3.dump ());

  t.ok (p3.skipWS (),          "skipWS ' three' --> true"); 

  t.ok (p3.getUntilWS (value), "getUntilWS 'three' --> true");
  t.is (value, "three",        "getUntilWS 'three' --> 'three'");
  t.is (p3.dump (),            "≪one two three≫ l13 c13", "dump: " + p3.dump ());

  // Pig::getDigit
  Pig p4 (" 123");
  int n;
  t.notok (p4.getDigit (n), "getDigit ' 123' --> false");
  t.ok (p4.skipWS (),       "skipWS ' 123' --> true");
  t.is (p4.dump (),         "≪ 123≫ l4 c1", "dump: " + p4.dump ());
  t.ok (p4.getDigit (n),    "getDigit '123' --> true");
  t.is (n, 1,               "getDigit '123' --> '1'");
  t.is (p4.dump (),         "≪ 123≫ l4 c2", "dump: " + p4.dump ());

  // Pig::getDigits
  Pig p5 ("123 ");
  t.ok (p5.getDigits (n),   "getDigits '123 ' --> true");
  t.is (n, 123,             "getDigits '123 ' --> 123");
  t.is (p5.dump (),         "≪123 ≫ l4 c3", "dump: " + p5.dump ());

  Pig p6 ("1");
  t.notok (p6.eos (),       "eos '1' --> false");
  t.ok (p6.getDigit (n),    "getDigit '1' --> true");
  t.notok (p6.getDigit (n), "getDigit '' --> false");
  t.ok (p6.eos (),          "eos '' --> true");
  t.is (p6.dump (),         "≪1≫ l1 c1", "dump: " + p6.dump ());

  // Pig::getNumber
  Pig p7 ("1 ");
  t.ok (p7.getNumber (value), "getNumber '1 ' --> true");
  t.is (value, "1",           "getNumber '1 ' --> '1'");
  t.is (p7.dump (),           "≪1 ≫ l2 c1", "dump: " + p7.dump ());

  Pig p8 ("3.14");
  t.ok (p8.getNumber (value), "getNumber '3.14' --> true");
  t.is (value, "3.14",        "getNumber '3.14' --> '3.14'");
  t.is (p8.dump (),           "≪3.14≫ l4 c4", "dump: " + p8.dump ());

  Pig p9 ("1.23e-4 ");
  t.ok (p9.getNumber (value), "getNumber '1.23e-4 ' --> true");
  t.is (value, "1.23e-4",     "getNumber '1.23e-4 ' --> '1.23e-4'");
  t.is (p9.dump (),           "≪1.23e-4 ≫ l8 c7", "dump: " + p9.dump ());

  Pig p10 ("2.34e-5");
  double dvalue;
  t.ok (p10.getNumber (dvalue), "getNumber '2.34e-5' --> true");
  t.is (dvalue, 2.34e-5, 1e-6,  "getNumber '2.34e-5' --> 2.34e-5 +/- 1e-6");
  t.is (p10.dump (),            "≪2.34e-5≫ l7 c7", "dump: " + p10.dump ());

  // Pig::getRemainder
  Pig p11 ("123");
  t.ok (p11.skipN (1),       "skipN=1 '123' --> true");
  t.is (p11.dump (),         "≪123≫ l3 c1", "dump: " + p11.dump ());

  t.ok (p11.getRemainder (value), "getRemainder '23' --> true");
  t.is (value, "23",        "getRemainder '23' --> '23'");
  t.is (p11.dump (),         "≪123≫ l3 c3", "dump: " + p11.dump ());

  t.notok (p11.getRemainder (value), "getRemainder '' --> false");
  t.is (p11.dump (),         "≪123≫ l3 c3", "dump: " + p11.dump ());

  // Pig::peek
  Pig p12 ("123");
  t.is (p12.peek (), '1',   "peek '123' --> '1'");
  t.is (p12.dump (),        "≪123≫ l3 c0", "dump: " + p12.dump ());
  t.is (p12.peek (2), "12",  "peek=2 '123' --> '12'");
  t.is (p12.dump (),        "≪123≫ l3 c0", "dump: " + p12.dump ());

  // Pig::save, Pig::restore
  Pig p13 ("123");
  t.is ((int)p13.save (), 0,    "save '123' --> 0");
  t.ok (p13.skipN (2),          "skipN=2 '123' --> true");
  t.is (p13.dump (),            "≪123≫ l3 c2", "dump: " + p13.dump ());
  t.is ((int)p13.cursor (), 2,  "cursor '123' --> 2");
  t.is ((int)p13.restore (), 0, "restore '123' --> 0");
  t.is (p13.dump (),            "≪123≫ l3 c0", "dump: " + p13.dump ());

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
