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

////////////////////////////////////////////////////////////////////////////////
// Use a default palette, which is overwritten in ::initialize.
Palette::Palette ()
{
  _colors = {
    Color ("white on red"),
    Color ("white on blue"),
    Color ("black on green"),
    Color ("black on magenta"),
    Color ("black on cyan"),
    Color ("black on yellow"),
    Color ("black on white"),
    Color ("white on bright red"),
    Color ("white on bright blue"),
    Color ("black on bright green"),
    Color ("black on bright magenta"),
    Color ("black on bright cyan"),
    Color ("black on bright yellow"),
  };
}

////////////////////////////////////////////////////////////////////////////////
void Palette::initialize (const Rules& rules)
{
  auto themeColors = rules.all ("theme.palette.color");
  if (themeColors.size ())
  {
    _colors.clear ();
    for (auto& entry : themeColors)
      _colors.push_back (Color (rules.get (entry)));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Return the next color in the list.  Cycle to the beginning if necessary.
Color Palette::next ()
{
  if (enabled)
    return _colors[_current++ % _colors.size ()];

  return Color ();
}

////////////////////////////////////////////////////////////////////////////////
int Palette::size () const
{
  return static_cast <int> (_colors.size ());
}

////////////////////////////////////////////////////////////////////////////////
