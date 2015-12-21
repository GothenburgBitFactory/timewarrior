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
#include <Grammar.h>
#include <Lexer.h>
#include <text.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
Grammar::Grammar ()
{
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::loadFromFile (File& file)
{
  if (file.exists ())
  {
    std::string contents;
    file.read (contents);
    loadFromString (contents);
  }
  else
    throw format ("Grammar file '{1}' not found.", static_cast<std::string> (file));
}

////////////////////////////////////////////////////////////////////////////////
// Load and parse BNF.
//
// Syntax:
//   rule-name:  alternate1-token1 alternate1-token2
//               alternate2-token1
//
// - Rules are aligned at left margin only, followed by a comma.
// - Productions are indented and never at left margin.
// - Blank line between rules.
//
// Details:
// - Literals are always double-quoted.
// - "*", "+" and "?" suffixes have POSIX semantics.
// - "є" means empty set.
// - Literal modifiers:
//   - :a  Accept abbreviations
//   - :i  Accept caseless match
//
void Grammar::loadFromString (const std::string& input)
{
  std::string rule_name = "";
  int token_count = 0;

  for (auto& line : split (input, '\n'))
  {
    // Skip whole-line comments.
    if (line[0] == '#')
      continue;

    // Eliminate inline comments.
    std::string::size_type hash = line.find ('#');
    if (hash != std::string::npos)
      line.resize (hash);

    line = Lexer::trim (line);

    // Skip blank lines with no semantics.
    if (line == "" and rule_name == "")
      continue;

    if (line != "")
    {
      token_count = 0;

      Lexer l (line);
      Lexer::Type type;
      std::string token;
      while (l.token (token, type))
      {
        ++token_count;

        if (token.back () == ':')
        {
          rule_name = token.substr (0, token.size () - 1);
          if (_start == "")
            _start = rule_name;

          _rules[rule_name] = Grammar::Rule ();
          token_count = 0;
        }
        else if (token.front () == ':')
        {
        }
        else
        {
          if (token_count <= 1)
            _rules[rule_name].push_back (Grammar::Production ());

          _rules[rule_name].back ().push_back (token);
        }
      }
    }
    else
      rule_name = "";
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string Grammar::dump () const
{
  std::stringstream out;
  for (auto& rule : _rules)
  {
    if (rule.first == _start)
      out << "▶ ";

    out << rule.first << ":\n";

    for (auto& production : rule.second)
    {
      out << "    ";
      for (auto& term : production)
        out << term << " ";

      out << "\n";
    }

    out << "\n";
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
