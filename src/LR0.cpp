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
void LR0::initialize (const Grammar& grammar)
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
/*
  LR0::Closure closure;
  for (unsigned int i = 0; i < augmented.size (); ++i)
    closure.push_back (LR0::Item (i, 0));
  LR0::States states;
  states.push_back (closure);
*/
  // Iteratively expand non-terminals until there are no more.
/*
  int state = 0;
  while (expandNonTerminals (augmented, states, state))
    ;
*/
}

////////////////////////////////////////////////////////////////////////////////
/*
bool LR0::expandNonTerminals (
  std::vector <std::vector <std::string>>& augmented,
  LR0::States& states,
  int state)
{
  if (_debug)
    std::cout << "Expand state " << state << ":\n"
              << dump (augmented, states);

  // TODO Find cases of ". <non-terminal>", and add a state for each unique <non-terminal>.

  return false;
}
*/

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
LR0::Item::Item (const std::vector <std::string>& rule)
: _rule (rule)
, _cursor (2)
{
  if (_rule.size () == 3 && _rule[2] == "є")
    _rule.pop_back ();
}

////////////////////////////////////////////////////////////////////////////////
bool LR0::Item::advance ()
{
  if (_cursor >= _rule.size ())
    return false;

  ++_cursor;
  return true;
}

////////////////////////////////////////////////////////////////////////////////
std::string LR0::Item::dump () const
{
  std::stringstream out;

  for (unsigned int i = 0; i < _rule.size (); ++i)
  {
    if (i)
      out << " ";

    if (i == _cursor)
      out << "● ";

    out << _rule[i];
  }

  if (_cursor >= _rule.size ())
    out << " ●";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
