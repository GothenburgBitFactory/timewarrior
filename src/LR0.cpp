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

    std::cout << "\n";
  }

  // Add all items from augmented grammar, in initial state:
  //   A --> . B ....
  std::vector <std::pair <int, int>> items;
  for (unsigned int i = 0; i < augmented.size (); ++i)
    items.push_back (std::pair <int, int> (i, 0));

  // Iteratively expand non-terminals until there are no more.
  while (expandNonTerminals (augmented, items))
    ;
}

////////////////////////////////////////////////////////////////////////////////
bool LR0::expandNonTerminals (
  std::vector <std::vector <std::string>>& augmented,
  std::vector <std::pair <int, int>>& items)
{
  if (_debug)
  {
    std::cout << "Expand:\n";
    int count = 0;
    for (auto& item : items)
    {
      std::cout << "  [" << count++ << "] "
                << augmented[item.first][0]
                << " "
                << augmented[item.first][1];

      for (int i = 2; i < (int)augmented[item.first].size (); ++i)
      {
        if (i - 2 == item.second)
          std::cout << " .";

        std::cout << " " << augmented[item.first][i];
      }

      std::cout << "\n";
    }

    std::cout << "\n";
  }



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
std::string LR0::dump (std::vector <std::vector <std::string>>& augmented, States& states) const
{
  std::stringstream out;

  for (unsigned int c = 0; c < states.size (); ++c)
  {
    out << "  [" << c << "] ";

    for (unsigned int i = 0; i < states[c].size (); ++i)
    {
      if (i)
        out << "      ";

      out << augmented[states[c][i]._rule][0]
          << " "
          << augmented[states[c][i]._rule][1];

      for (unsigned int t = 2; t < augmented[states[c][i]._rule].size (); t++)
      {
        if ((int)t - 2 == states[c][i]._cursor)
          out << " .";

        out << " " << augmented[states[c][i]._rule][t];
      }

      out << "\n";
    }

    out << "\n";
  }

  out << "\n";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
