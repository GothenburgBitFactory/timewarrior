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
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
Grammar::Grammar ()
: _debug (false)
{
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::loadFromFile (File& file)
{
  if (! file.exists ())
    throw format ("Grammar file '{1}' not found.", file._data);

  std::string contents;
  file.read (contents);
  loadFromString (contents);
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

  // This is a state machine.  Read each line.
  for (auto& line : split (input, '\n'))
  {
    // Skip whole-line comments.
    if (line[0] == '#')
      continue;

    // Eliminate inline comments.
    std::string::size_type hash = line.find ('#');
    if (hash != std::string::npos)
      line.resize (hash);

    // Skip blank lines with no semantics.
    line = Lexer::trim (line);
    if (line == "" and rule_name == "")
      continue;

    if (line != "")
    {
      int token_count = 0;

      Lexer l (line);
      Lexer::Type type;
      std::string token;
      while (l.token (token, type))
      {
        ++token_count;

        if (token.back () == ':')
        {
          // Capture the Rule_name.
          rule_name = token.substr (0, token.size () - 1);

          // If this is the first Rule, capture it as a starting point.
          if (_start == "")
            _start = rule_name;

          _rules[rule_name] = Grammar::Rule ();
          token_count = 0;
        }
        else if (token.front () == ':')
        {
          // Decorate the most recent token, of the most recent Production,
          // of the current Rule.
          _rules[rule_name].back ().back ().decorate (token);
        }
        else
        {
          // If no Production was added yet, add one.
          if (token_count <= 1)
            _rules[rule_name].push_back (Grammar::Production ());

          // Add the new Token to the most recent Production, of the current
          // Rule.
          _rules[rule_name].back ().push_back (Grammar::Token (token));
        }
      }
    }

    // A blank line in the input ends the current rule definition.
    else
      rule_name = "";
  }

  if (_debug)
    std::cout << dump () << "\n";

  // Validate the parsed grammar.
  validate ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Grammar::start () const
{
  return _start;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Grammar::rules () const
{
  std::vector <std::string> results;
  for (auto& rule : _rules)
    results.push_back (rule.first);

  return results;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Grammar::terminals () const
{
  std::vector <std::string> results;
  for (auto& rule : _rules)
    for (auto& production : rule.second)
      for (auto& token : production)
        if (_rules.find (token._token) == _rules.end ())
          results.push_back (token._token);

  return results;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::vector <std::string>> Grammar::augmented () const
{
  std::vector <std::vector <std::string>> results {{"S", "-->", _start}};
  for (auto& rule : _rules)
    for (auto& production : rule.second)
    {
      std::vector <std::string> terms;
      terms.push_back (rule.first);
      terms.push_back ("-->");
      for (auto& token : production)
        terms.push_back (token._token);

      results.push_back (terms);
    }

  return results;
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::debug (bool value)
{
  _debug = value;
}

////////////////////////////////////////////////////////////////////////////////
std::string Grammar::dump () const
{
  std::stringstream out;
  out << "Grammar\n";
  for (auto& rule : _rules)
  {
    // Indicate the start Rule.
    out << "  " << (rule.first == _start ? "▶" : " ") << " " << rule.first << ": ";

    int count = 0;
    for (auto& production : rule.second)
    {
      if (count)
        out << "| ";

      for (auto& token : production)
      {
        out << token._token;
        if (token._decoration != "")
          out << " " << token._decoration;
        out << " ";
      }

      ++count;
    }

    out << "\n";
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
void Grammar::validate () const
{
  if (_debug)
    std::cout << "Validating...\n";

  if (_start == "")
    throw std::string ("There are no rules defined.");

  std::vector <std::string> allRules;
  std::vector <std::string> allTokens;
  std::vector <std::string> allLeftRecursive;

  for (auto& rule : _rules)
  {
    allRules.push_back (rule.first);

    for (auto& production : rule.second)
    {
      for (auto& token : production)
      {
        if (token._token.front () != '"' and
            token._token.front () != '/')
          allTokens.push_back (token._token);

        if (token._token == production[0]._token &&
            rule.first == production[0]._token   &&
            production.size () == 1)
          allLeftRecursive.push_back (token._token);
      }
    }
  }

  std::vector <std::string> notUsed;
  std::vector <std::string> notDefined;
  listDiff (allRules, allTokens, notUsed, notDefined);

  // Undefined value - these are definitions that appear in token, but are
  // not in _rules.
  for (auto& nd : notDefined)
    if (nd != "є")
      throw format ("Definition '{1}' referenced, but not defined.", nd);

  // Circular definitions - these are names in _rules that also appear as
  // the only token in any of the alternates for that definition.
  for (auto& lr : allLeftRecursive)
    throw format ("Definition '{1}' is left recursive.", lr);

  for (auto& r : allRules)
    if (r[0] == '"' or
        r[0] == '/')
      throw format ("Definition '{1}' must not be a literal.");

  // Unused definitions - these are names in _rules that are never
  // referenced as token.
  for (auto& nu : notUsed)
    if (nu != _start)
      throw format ("Definition '{1}' is defined, but not referenced.", nu);
}

////////////////////////////////////////////////////////////////////////////////
