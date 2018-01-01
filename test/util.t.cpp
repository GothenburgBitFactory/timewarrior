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
  UnitTest t (7);

  // std::string escape (const std::string& input, int c)
  t.is (escape ("", 'x'),    "",        "escape '','x' --> ''");
  t.is (escape ("foo", 'x'), "foo",     "escape 'foo','x' --> 'foo'");
  t.is (escape ("foo", 'f'), "\\foo",   "escape 'foo','f' --> '\\foo'");
  t.is (escape ("foo", 'o'), "f\\o\\o", "escape 'foo','o' --> 'f\\o\\o'");
  t.is (escape ("\"", '"'),  "\\\"",    "escape '\"','\"' --> '\\\"'");

  // std::string quoteIfNeeded (const std::string& input)
  t.is (quoteIfNeeded ("foo"), "foo",         "quoteIfNeeded 'foo' --> 'foo'");
  t.is (quoteIfNeeded ("f o o"), "\"f o o\"", "quoteIfNeeded 'f o o' --> '\"f o o\"'");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

