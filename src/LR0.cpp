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
// Aho/Sethi/Ullman, p224
// The sets-of-items construction:
//
// procedure items(G');
// begin
//   C := {closure ({[S' --> . S]})};
//   repeat
//     for each set of items I in C and each grammar symbol X such that
//     goto (I, X) is not empty and not in C do
//       add goto (I, X) to C
//   until no more sets of items can be added to C
// end
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

  // TODO Expand this single interation into a loop that ends when there is no
  // more expanѕion.
  Item item0 {augmented[0]};
  auto closure = getClosure (item0, augmented);

  States states;
  states.push_back (closure);
  std::cout << states.dump () << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// Aho/Sethi/Ullman, p223
//
// function closure (I);
// begin
//   J := I;
//   repeat
//     for each item A --> alpha . B beta in J and each production
//       B --> gamma of G such that B -> . gamma is not in J do
//         add B --> . gamma to J
//   until no more items can be added to J;
//   return J
// end
LR0::Closure LR0::getClosure (
  const Item& item,
  const std::vector <std::vector <std::string>>& augmented)
{
  LR0::Closure closure;
  closure.push_back (item);

  // TODO Add the rest

  return closure;
}

////////////////////////////////////////////////////////////////////////////////
// Aho/Sethi/Ullman, p224
//
// If I is the set of two items {[E' --> E .], [E --> E . + T]}, then
// goto (I, +) consists of:
//
// E --> E + . T
// T --> . T * F
// T --> . F
// F --> . ( E )
// F --> . id
//
// We computed goto (I, +) by examining I for items with + immediately to the
// right of the dot. E' --> E . is not such an item, but E --> E . + T is.
// We moved the dot over the + to get {E --> E + . T} and then took the closure
// of this set.

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
  //  +-------+--+--+--+--+---+----+-----+----+
  //  |       |  |  |  |  |   |    |     |    |
  //  +-------+--+--+--+--+---+----+-----+----+
  //  |       |  |  |  |  |   |    |     |    |
  //  +-------+--+--+--+--+---+----+-----+----+

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
std::string LR0::States::dump () const
{
  std::stringstream out;
  out << "States\n";

  for (unsigned int state = 0; state < this->size (); ++state)
  {
    out << "  State " << state << "\n";

    for (auto& item : (*this)[state])
      out << "    " << item.dump () << "\n";
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
