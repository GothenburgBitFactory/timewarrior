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
#include <LR0.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (13);

  // LR0::Item.
  LR0::Item item1 ({"E", "-->", "E", "*", "B"});
  t.is (item1.dump (), "E --> ● E * B", "E --> ● E * B");

  t.ok (item1.advance (), "Item::advance true");
  t.is (item1.dump (), "E --> E ● * B", "E --> E ● * B");

  t.ok (item1.advance (), "Item::advance true");
  t.is (item1.dump (), "E --> E * ● B", "E --> E * ● B");

  t.notok (item1.done (), "Item::done false");
  t.ok (item1.advance (), "Item::advance true");
  t.ok (item1.done (), "Item::done true");
  t.is (item1.dump (), "E --> E * B ●", "E --> E * B ●");

  t.notok (item1.advance (), "Item::advance false");

  // LR0::Item special case for "A --> є".
  LR0::Item item2 ({"A", "-->", "є"});
  t.is (item2.dump (), "A --> ●", "A --> ●");
  t.notok (item2.advance (), "Item::advance false");
  t.ok (item2.done (), "Item::done true");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
