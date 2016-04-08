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
#include <Datetime.h>
#include <shared.h>
#include <format.h>

////////////////////////////////////////////////////////////////////////////////
// An exclusion represents untrackable time such as holidays, weekends, evenings
// and lunch. By default there are none, but they may be configured. Once there
// are exclusions defined, the :fill functionality is enabled.
//
// Exclusions are instantiated from the 'define exclusions:' rule. This method
// simply validates.
//
// Syntax:
//   exc monday 8:00:00-12:00:00 12:45:00-17:30:00
//   exc day on 2016-01-01
//   exc day off 2016-01-02

void Exclusion::initialize (const std::string& line)
{
  _tokens = split (line);

  // Validate syntax only. Do nothing with the data.
  if (_tokens.size () >= 2 &&
      _tokens[0] == "exc")
  {
    if (_tokens.size () == 4 &&
        _tokens[1] == "day"  &&
        (_tokens[2] == "on" || _tokens[2] == "off"))
    {
      return;
    }
    else if (Datetime::dayOfWeek (_tokens[1]) != -1)
             // TODO Check the time range args.
    {
      return;
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
std::string Exclusion::dump () const
{
  std::stringstream out;

  out << "Exclusion _tokens ";
  for (auto& token : _tokens)
    out << " '" << token << "'";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
