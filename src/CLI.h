////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2006 - 2016, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_CLI
#define INCLUDED_CLI

#include <Lexer.h>
#include <string>
#include <vector>
#include <map>

// Represents a single argument.
class A2
{
public:
  A2 (const std::string&, Lexer::Type);
  bool hasTag (const std::string&) const;
  void tag (const std::string&);
  void unTag (const std::string&);
  void attribute (const std::string&, const std::string&);
  const std::string attribute (const std::string&) const;
  const std::string getToken () const;
  const std::string dump () const;

public:
  Lexer::Type                         _lextype     {Lexer::Type::word};
  std::vector <std::string>           _tags        {};
  std::map <std::string, std::string> _attributes  {};
};

// Represents the command line.
class CLI
{
public:
  CLI () = default;
  void entity (const std::string&, const std::string&);
  void add (const std::string&);
  void analyze ();
  bool canonicalize (std::string&, const std::string&, const std::string&) const;
  const std::string dump (const std::string& title = "CLI Parser") const;

private:
  void handleArg0 ();

public:
  std::multimap <std::string, std::string>           _entities             {};
  std::vector <A2>                                   _original_args        {};
  std::vector <A2>                                   _args                 {};
};

#endif

