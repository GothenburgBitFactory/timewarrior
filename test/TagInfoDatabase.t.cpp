////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <test.h>
#include <TagInfoDatabase.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (9);


  TagInfoDatabase tagInfoDatabase {};

  t.is (tagInfoDatabase.incrementTag ("foo"), -1, "First insertion of 'foo' returns -1");
  t.is (tagInfoDatabase.incrementTag ("foo"), 1, "New insertion of 'foo' increments count");
  t.is (tagInfoDatabase.incrementTag ("bar"), -1, "First insertion of 'bar' returns -1");
  t.is (tagInfoDatabase.incrementTag ("bar"), 1, "New insertion of 'bar' increments count");
  t.is (tagInfoDatabase.decrementTag ("foo"), 1, "Removal of 'foo' decrements count");
  t.is (tagInfoDatabase.decrementTag ("bar"), 1, "Removal of 'bar' decrements count");
  t.is (tagInfoDatabase.decrementTag ("foo"), 0, "Deletion of 'foo' returns 0");
  t.is (tagInfoDatabase.decrementTag ("bar"), 0, "Deletion of 'bar' returns 0");

  try
  {
    tagInfoDatabase.decrementTag ("xyz");
    t.fail ("Deletion of non-existent tag throws an exception");
  }
  catch (...)
  {
    t.pass ("Deletion of non-existent tag throws an exception");
  }

  return 0;
}



