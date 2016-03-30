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

////////////////////////////////////////////////////////////////////////////////
// Initially assume no text, but infinite virtual space.
//
// Ållow overlay placement of arbitrary text at any offset, real or virtual, and
// using a specific color. The color is blended with any underlying color.
//
// Collapse all the strings down to a single string, with the most efficient
// color directives inline.
//
// For example:
//   Composite c;
//   c.add ("first",  2, Color ("red"));
//   c.add ("second", 5, Color ("underline"));
//
// Result:
//   "  firsecond"
//      rrrrr
//         uuuuuu
//
// The first part "fir" will be red.
// The second part "se" will be red and underlined.
// The third part "cond" will be underlined.
//
void Composite::add (
  const std::string& text,
  std::string::size_type offset,
  const Color& color)
{
  _layers.push_back (std::make_tuple (text, offset, color));
}

////////////////////////////////////////////////////////////////////////////////
// overlay == true    means there is no color blending.
// overlay == false   means there is color blending.
std::string Composite::str (bool overlay)
{
  return "";
}

////////////////////////////////////////////////////////////////////////////////
