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

#ifndef INCLUDED_LR0
#define INCLUDED_LR0

#include <Grammar.h>
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
    bool advance ();
    std::string dump () const;

  private:
    std::vector <std::string> _rule;
    unsigned int _cursor;
  };

  class Closure : public std::vector <Item>
  {
  };

  class States : public std::vector <Closure>
  {
  };

private:
  Closure getClosure (const Item&, const std::vector <std::vector <std::string>>&);

private:
  //        state         column        result
  //        |              |            |
  std::map <int, std::map <std::string, std::string>> _actions;
  std::map <int, std::map <std::string, std::string>> _goto;

  bool _debug;
};

#endif

////////////////////////////////////////////////////////////////////////////////
