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
#include <Composite.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (2);

  Composite c1;
  c1.add ("left",  2, Color ());
  c1.add ("right", 4, Color ());
  t.is (c1.str (), "  leright", "Composite left/2 + right/4 --> '  leright'");

  Composite c2;
  c2.add ("left",  2, Color ("white on red"));
  c2.add ("right", 4, Color ("white on blue"));
  t.diag (c2.str ());

  Composite c3;
  c3.add ("aaaaaaaaaa",  2, Color ());
  c3.add ("bbbbb",       5, Color ());
  c3.add ("c",          15, Color ());
  t.is (c3.str (), "  aaabbbbbaa   c", "Composite aaaaaaaaaa/2 + bbbbb/5 + c/15 --> '  aaabbbbbaa   c'");

  Composite c4;
  c4.add ("aaaaaaaaaa",  2, Color ("white on red"));
  c4.add ("bbbbb",       5, Color ("white on blue"));
  c4.add ("c",          15, Color ("white on green"));
  t.diag (c4.str ());

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

