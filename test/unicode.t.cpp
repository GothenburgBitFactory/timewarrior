////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013 - 2016, Göteborg Bit Factory.
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
#include <unicode.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (32);

  // White space detection.
  t.notok (unicodeWhitespace (0x0041), "U+0041 (A) ! unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x0020), "U+0020 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x0009), "U+0009 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x000A), "U+000A unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x000B), "U+000B unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x000C), "U+000C unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x000D), "U+000D unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x0085), "U+0085 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x00A0), "U+00A0 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x1680), "U+1680 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x180E), "U+180E unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2000), "U+2000 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2001), "U+2001 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2002), "U+2002 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2003), "U+2003 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2004), "U+2004 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2005), "U+2005 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2006), "U+2006 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2007), "U+2007 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2008), "U+2008 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2009), "U+2009 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x200A), "U+200A unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2028), "U+2028 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x2029), "U+2029 unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x202F), "U+202F unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x205F), "U+205F unicodeWhitespace");
  t.ok    (unicodeWhitespace (0x3000), "U+3000 unicodeWhitespace");

  // Latin Alpha
  t.notok (unicodeLatinAlpha (0x0033), "U+0033 (3) ! unicodeLatinAlpha");
  t.ok    (unicodeLatinAlpha (0x0041), "U+0041 (A) unicodeLatinAlpha");
  t.ok    (unicodeLatinAlpha (0x005A), "U+005A (Z) unicodeLatinAlpha");
  t.ok    (unicodeLatinAlpha (0x0061), "U+0061 (a) unicodeLatinAlpha");
  t.ok    (unicodeLatinAlpha (0x007A), "U+007A (z) unicodeLatinAlpha");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
