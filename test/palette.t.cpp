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
#include <Palette.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (3);

  Palette p;
  t.ok (p.size () >= 8, "Palette.size at least 8");

  p.enabled = true;
  t.ok (p.next () == Color ("white on red"), "Palette provides color when enabled");
  p.enabled = false;
  t.ok (p.next () == Color (),               "Palette suppresses color when disabled");
  p.enabled = true;

  t.diag (p.next ().colorize ("palette entry 0"));
  t.diag (p.next ().colorize ("palette entry 1"));
  t.diag (p.next ().colorize ("palette entry 2"));
  t.diag (p.next ().colorize ("palette entry 3"));
  t.diag (p.next ().colorize ("palette entry 4"));
  t.diag (p.next ().colorize ("palette entry 5"));
  t.diag (p.next ().colorize ("palette entry 6"));
  t.diag (p.next ().colorize ("palette entry 7"));
  t.diag (p.next ().colorize ("palette entry 8"));
  t.diag (p.next ().colorize ("palette entry 9"));
  t.diag (p.next ().colorize ("palette entry 10"));
  t.diag (p.next ().colorize ("palette entry 11"));
  t.diag (p.next ().colorize ("palette entry 12"));
  t.diag ("Repeating...");
  t.diag (p.next ().colorize ("palette entry 1"));
  t.diag (p.next ().colorize ("palette entry 2"));

  t.diag ("Disabled...");
  p.enabled = false;
  t.diag (p.next ().colorize ("palette entry 1"));
  t.diag (p.next ().colorize ("palette entry 2"));
  p.enabled = true;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

