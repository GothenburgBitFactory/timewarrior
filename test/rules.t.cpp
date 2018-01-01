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
#include <Rules.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (8);

  Rules r;
  r.set ("string", "234");
  t.is (r.get ("string"),         "234", "Rules set string, get string");
  t.is (r.getInteger ("string"),  234,   "Rules set string, get string");

  // Throws on error.
  r.set ("undefined", "foo");
  try {r.getInteger ("undefined", -1);  t.fail ("Rules set non-int string, get 0");}
  catch (...)                          {t.pass ("Rules set non-int string, get 0");}

  t.is (r.getBoolean ("undefined"), false, "Rules set non-bool string, get false");

  r.set ("integer", 123);
  t.is (r.getInteger ("integer"), 123,   "Rules set integer, get integer");
  t.is (r.get        ("integer"), "123", "Rules set integer, get string");

  r.set ("one.two",       12);
  r.set ("one.two.three", 123);
  r.set ("one.two.four",  124);
  t.ok ((int) r.all ().size () > 30,     "Rules all (\"\") --> >30");
  t.ok (r.all ("one.two").size () == 3,  "Rules all (\"one.two\") --> 3");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

