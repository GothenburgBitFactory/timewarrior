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

#ifndef INCLUDED_LR0
#define INCLUDED_LR0

#include <Grammar.h>
#include <set>
#include <map>
#include <string>

class LR0
{
public:
  LR0 ();
  void initialize (const Grammar&);
  void parse (const std::string&);
  void debug (bool);
  std::string dump () const;

  class Item
  {
  public:
    Item (const std::vector <std::string>&);
    void setGrammarRuleIndex (const int);
    bool advance ();
    bool done () const;
    std::string next () const;
    std::string dump () const;

  private:
    std::vector <std::string> _rule;
    unsigned int _cursor;
    int _grammarRule;
  };

  class Closure : public std::vector <Item>
  {
  };

  class States : public std::vector <Closure>
  {
  public:
    std::string dump () const;
  };

private:
  std::set <std::string> getExpectedSymbols (const Closure&) const;
  Closure getClosure (const std::string&) const;
  bool closeState (States&, const int) const;

private:
  // Copy of the augmented grammar.
  std::vector <std::vector <std::string>> _augmented;

  //        state        column       result
  //        |            |            |
  std::vector <std::map <std::string, std::string>> _actions;
  std::vector <std::map <std::string, std::string>> _goto;

  bool _debug;
};

#endif

////////////////////////////////////////////////////////////////////////////////
