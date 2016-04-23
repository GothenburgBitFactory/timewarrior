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
#include <utf8.h>
#include <sstream>
#include <stack>

////////////////////////////////////////////////////////////////////////////////
// Initially assume no text, but infinite virtual space.
//
// Ållow overlay placement of arbitrary text at any offset, real or virtual, and
// using a specific color.
//
// For example:
//   Composite c;
//   c.add ("aaaaaaaaaa",  2, Color ("..."));    // Layer 1
//   c.add ("bbbbb",       5, Color ("..."));    // Layer 2
//   c.add ("c",          15, Color ("..."));    // Layer 3
//
//   _layers = { std::make_tuple ("aaaaaaaaaa",  2, Color ("...")),
//               std::make_tuple ("bbbbb",       5, Color ("...")),
//               std::make_tuple ("c",          15, Color ("..."))};
//
void Composite::add (
  const std::string& text,
  std::string::size_type offset,
  const Color& color)
{
  _layers.push_back (std::make_tuple (text, offset, color));
}

////////////////////////////////////////////////////////////////////////////////
// Merge the layers of text and color into one string.
//
// For example:
//   Composite c;
//   c.add ("aaaaaaaaaa",  2, Color ("..."));    // Layer 1
//   c.add ("bbbbb",       5, Color ("..."));    // Layer 2
//   c.add ("c",          15, Color ("..."));    // Layer 3
//
//   _layers = { std::make_tuple ("aaaaaaaaaa",  2, Color ("...")),
//               std::make_tuple ("bbbbb",       5, Color ("...")),
//               std::make_tuple ("c",          15, Color ("..."))};
//
// Arrange strings conceptually:
//              111111
//    0123456789012345     // Position
//
//      aaaaaaaaaa         // Layer 1
//         bbbbb           // Layer 2
//                   c     // Layer 3
//
// Walk all strings left to right, selecting the character and color from the
// highest numbered layer. Emit color codes only on edge detection.
//
std::string Composite::str (bool blend) const
{
  // The strings are broken into a vector of int, for UTF8 support.
  std::vector <int> characters;
  std::vector <int> colors;
  for (unsigned int layer = 0; layer < _layers.size (); ++layer)
  {
    auto text   = std::get <0> (_layers[layer]);
    auto offset = std::get <1> (_layers[layer]);

    auto len    = utf8_text_length (text);

    // Make sure the vectors are large enough to support a write operator[].
    if (characters.size () < offset + len)
    {
      characters.resize (offset + len, 32);
      colors.resize     (offset + len, 0);
    }

    // Copy in the layer characters and color indexes.
    std::string::size_type cursor = 0;
    int character;
    int count = 0;
    while ((character = utf8_next_char (text, cursor)))
    {
      characters[offset + count] = character;
      colors    [offset + count] = layer + 1;
      ++count;
    }
  }

  // Now walk the character and color vector, emitting every character and
  // every detected color change.
  std::stringstream out;
  int prev_color = 0;
  for (unsigned int i = 0; i < characters.size (); ++i)
  {
    // A change in color triggers a code emit.
    if (prev_color != colors[i])
    {
      out << std::get <2> (_layers[colors[i]]).code ();
      prev_color = colors[i];
    }

    out << utf8_character (characters[i]);
  }

  // Terminate the color codes.
  out << std::get <2> (_layers[0]).end ();

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
