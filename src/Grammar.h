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

#ifndef INCLUDED_GRAMMAR
#define INCLUDED_GRAMMAR

#include <FS.h>
#include <string>
#include <vector>
#include <map>

class Grammar
{
public:
  Grammar ();
  void loadFromFile (File&);
  void loadFromString (const std::string&);
  std::string start () const;
  std::vector <std::string> rules () const;
  std::vector <std::string> terminals () const;
  std::map <std::string, std::vector <std::string>> items () const;
  std::string dump () const;

protected:
  class Token
  {
  public:
    Token (const std::string& value)         { _token      = value; }
    void decorate (const std::string& value) { _decoration = value; }

    std::string _token;
    std::string _decoration;
  };

  class Production : public std::vector <Token>
  {
  };

  class Rule : public std::vector <Production>
  {
  };

private:
  void validate () const;

private:
  std::string _start;
  std::map <std::string, Grammar::Rule> _rules;
};

#endif

////////////////////////////////////////////////////////////////////////////////
