////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015, Paul Beckingham, Federico Hernandez.
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
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
LR0::LR0 ()
{
}

////////////////////////////////////////////////////////////////////////////////
void LR0::createParseTables (const Grammar& grammar)
{
  // Obtain the augmented grammar.
  auto augmented = grammar.augmented ();

  if (_debug)
  {
    std::cout << "Augmented Grammar\n";
    auto count = 0;
    for (auto& item : augmented)
    {
      std::cout << "  [" << count++ << "]";
      for (auto& term : item)
        std::cout << " " << term;
      std::cout << "\n";
    }
  }

  // TODO Add all items from augmented grammar.
  std::vector <std::pair <int, int>> items;
  // TODO Add augmented grammar to items.

  // TODO Add new states.
  while (expandNonTerminals ())
    ;
}

////////////////////////////////////////////////////////////////////////////////
bool LR0::expandNonTerminals ()
{



  return false;
}

////////////////////////////////////////////////////////////////////////////////
void LR0::parse (const std::string& input)
{
}

////////////////////////////////////////////////////////////////////////////////
void LR0::debug (bool value)
{
  _debug = value;
}

////////////////////////////////////////////////////////////////////////////////
std::string LR0::dump () const
{
  std::stringstream out;

  // TODO Render _actions and _goto as a table.
  //
  //  +-------+---------------+---------------+
  //  | state | actions...    | goto...       |
  //  |       | terminals   $ | non-terminals |
  //  +-------+-----------+---+----+-----+----+
  //  |       |           |   |    |     |    |
  //  +-------+-----------+---+----+-----+----+
  //  |       |           |   |    |     |    |
  //  +-------+-----------+---+----+-----+----+

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
