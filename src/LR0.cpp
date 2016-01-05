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
#include <LR0.h>
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
LR0::LR0 ()
{
}

////////////////////////////////////////////////////////////////////////////////
// Given a grammar:
// - Obtain the augmented grammar
// - Create the first state from the first augmented grammar rule
// - Expand the first state
// - Recursively create and expand all the other states
void LR0::initialize (const Grammar& grammar)
{
  // Obtain the augmented grammar.
  _augmented = grammar.augmented ();

  if (_debug)
  {
    std::cout << "Augmented Grammar\n";
    auto count = 0;
    for (auto& item : _augmented)
    {
      std::cout << "  [" << count++ << "]";
      for (auto& term : item)
        std::cout << " " << term;
      std::cout << "\n";
    }

    std::cout << "\n";
  }

  // Initialize the first state.
  States states;
  initializeFirstState (states);

  // Recursively fill in the rest.
  closeState (states, 0);
  std::cout << states.dump () << "\n";
}

////////////////////////////////////////////////////////////////////////////////
// Collect a unique set of expected symbols from the closure. This is the set
// of symbols where '● X' appears in an item.
std::set <std::string> LR0::getExpectedSymbols (const Closure& closure) const
{
  std::set <std::string> expected;
  for (auto& item : closure)
    if (! item.done ())
      expected.insert (item.next ());

  return expected;
}

////////////////////////////////////////////////////////////////////////////////
// Add all items to the result set matching the production rule for symbol.
LR0::Closure LR0::getClosure (const std::string& symbol) const
{
  std::set <std::string> seen;

  LR0::Closure result;
  for (unsigned int r = 0; r < _augmented.size (); ++r)
  {
    if (_augmented[r][0] == symbol)
    {
      Item item (_augmented[r]);
      item.setGrammarRuleIndex (r);
      result.push_back (item);
      seen.insert (_augmented[r][0]);

      auto nextSymbol = item.next ();
      if (seen.find (nextSymbol) == seen.end ())
      {
        for (unsigned int r = 0; r < _augmented.size (); ++r)
        {
          if (_augmented[r][0] == nextSymbol)
          {
            Item item (_augmented[r]);
            item.setGrammarRuleIndex (r);
            result.push_back (item);
            seen.insert (nextSymbol);
          }
        }
      }
    }
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// The first state is юust the augmented grammar with the expected symbols all
// at the first symbol of each rule, i.e. exactly what happens when an Item is
// instantiated from a grammar rule.
void LR0::initializeFirstState (States& states) const
{
  LR0::Closure result;
  for (unsigned int r = 0; r < _augmented.size (); ++r)
  {
    Item item (_augmented[r]);
    item.setGrammarRuleIndex (r);
    result.push_back (item);
  }

  states.push_back (result);
}

////////////////////////////////////////////////////////////////////////////////
void LR0::closeState (States& states, const int state) const
{
  // Obtain all the expected symbols for this state.
  auto expectedSymbols = getExpectedSymbols (states[state]);
  for (auto& expected : expectedSymbols)
  {
    // This will be the new state.
    Closure closure;

    // Track additional symbols.
    std::set <std::string> seen;

    // Find all the rules in this state that are expecting 'expected'.
    for (auto& item : states[state])
    {
      if (! item.done () &&
          item.next () == expected)
      {
        Item advanced (item);
        advanced.advance ();
        closure.push_back (advanced);

        if (! advanced.done ())
        {
          auto nextSymbol = advanced.next ();
          if (seen.find (nextSymbol) == seen.end ())
          {
            for (unsigned int r = 0; r < _augmented.size (); ++r)
            {
              if (_augmented[r][0] == nextSymbol)
              {
                Item additional (_augmented[r]);
                additional.setGrammarRuleIndex (r);
                closure.push_back (additional);
                seen.insert (nextSymbol);
              }
            }
          }
        }
      }
    }

    // Check that the new state is not already created.
    bool skip = false;
    for (auto& state : states)
      if (state[0] == closure[0])
        skip = true;

    // Create the new state, and recurse to close it.
    if (! skip)
    {
      states.push_back (closure);
      closeState (states, states.size () - 1);
    }
  }
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
, _grammarRule (-1)
{
  if (_rule.size () == 3 && _rule[2] == "є")
    _rule.pop_back ();
}

////////////////////////////////////////////////////////////////////////////////
bool LR0::Item::operator== (const Item& other)
{
  if (_cursor       != other._cursor      ||
      _grammarRule  != other._grammarRule ||
      _rule.size () != other._rule.size ())
    return false;

  for (unsigned int i = 0; i < _rule.size () - 1; ++i)
    if (_rule[i] != other._rule[i])
      return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////
void LR0::Item::setGrammarRuleIndex (const int rule)
{
  _grammarRule = rule;
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
bool LR0::Item::done () const
{
  return _cursor >= _rule.size ();
}

////////////////////////////////////////////////////////////////////////////////
std::string LR0::Item::next () const
{
  return _rule[_cursor];
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

  if (_grammarRule != -1)
    out << " [g" << _grammarRule << "]";

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
