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
#include <Exclusion.h>
#include <Lexer.h>
#include <Datetime.h>
#include <shared.h>
#include <format.h>

////////////////////////////////////////////////////////////////////////////////
// An exclusion represents untrackable time such as holidays, weekends, evenings
// and lunch. By default there are none, but they may be configured. Once there
// are exclusions defined, the :fill functionality is enabled.
//
// Exclusions are instantiated from the 'define exclusions:' rule.
//
// Syntax:
//   exc holidays en-US
//   exc work 2015-11-26
//   exc week mon,tue,wed,thu,fri
//   exc day start 8:30am
//   exc day end 1730
//   exc day tue end 3pm

void Exclusion::initialize (const std::string& line)
{
  Lexer lexer (line);
  std::string token;
  Lexer::Type type;
  while (lexer.token (token, type))
    _tokens.push_back (Lexer::dequote (token));

  if (_tokens.size () >= 3 &&
      _tokens[0] == "exc")
  {
    if (_tokens.size () == 3      &&
        _tokens[1] == "holidays"  &&
        _tokens[2].length () == 5 &&
        _tokens[2][2] == '-')
    {
      return;
    }
    else if (_tokens.size () == 3 &&
             _tokens[1] == "work")
    {
      return;
    }
    else if (_tokens.size () == 3 &&
             _tokens[1]      == "week")
    {
      for (auto& day : split (_tokens[2], ','))
        if (Datetime::dayOfWeek (day) == -1)
          throw format ("Unrecognized days in '{1}'", _tokens[2]);

      return;
    }
    else if (_tokens[1] == "day")
    {
      if (_tokens.size () == 4 &&
          (_tokens[2] == "start" || _tokens[2] == "end"))
      {
        return;
      }
      else if (_tokens.size () == 5                   &&
               Datetime::dayOfWeek (_tokens[2]) != -1 &&
               (_tokens[3] == "start" || _tokens[3] == "end"))
      {
        return;
      }
    }
  }

  throw format ("Unrecognized exclusion syntax: '{1}'.", line);
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Exclusion::tokens () const
{
  return _tokens;
}

////////////////////////////////////////////////////////////////////////////////
