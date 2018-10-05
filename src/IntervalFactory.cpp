////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <format.h>
#include <Lexer.h>
#include <IntervalFactory.h>
#include <JSON.h>

////////////////////////////////////////////////////////////////////////////////
// Syntax:
//   'inc' [ <iso> [ '-' <iso> ]] [ '#' <tag> [ <tag> ... ]]
Interval IntervalFactory::fromSerialization (const std::string &line)
{
  Lexer lexer (line);
  std::vector <std::string> tokens;
  std::string token;
  Lexer::Type type;

  while (lexer.token (token, type))
  {
    tokens.push_back (Lexer::dequote (token));
  }

  // Minimal requirement 'inc'.
  if (!tokens.empty () &&
      tokens[0] == "inc")
  {
    Interval interval = Interval ();

    unsigned int offset = 0;

    // Optional <iso>
    if (tokens.size () > 1 &&
        tokens[1].length () == 16)
    {
      interval.start = Datetime (tokens[1]);
      offset = 1;

      // Optional '-' <iso>
      if (tokens.size () > 3 &&
          tokens[2] == "-"   &&
          tokens[3].length () == 16)
      {
        interval.end = Datetime (tokens[3]);
        offset = 3;
      }
    }

    // Optional '#' <tag>
    if (tokens.size () > 2 + offset &&
        tokens[1 + offset] == "#")
    {
      // Optional <tag> ...
      for (unsigned int i = 2 + offset; i < tokens.size (); ++i)
        interval.tag (tokens[i]);
    }

    return interval;
  }

  throw format ("Unrecognizable line '{1}'.", line);
}

////////////////////////////////////////////////////////////////////////////////
Interval IntervalFactory::fromJson (std::string jsonString)
{
  Interval interval = Interval ();

  if (!jsonString.empty ())
  {
    auto* json = (json::object*) json::parse (jsonString);

    json::array* tags = (json::array*) json->_data["tags"];

    if (tags != nullptr)
    {
      for (auto& tag : tags->_data)
      {
        auto* value = (json::string*) tag;
        interval.tag(value->_data);
      }
    }

    json::string* start = (json::string*) json->_data["start"];
    interval.start = (start != nullptr) ? Datetime(start->_data) : 0;
    json::string* end = (json::string*) json->_data["end"];
    interval.end = (end != nullptr) ? Datetime(end->_data) : 0;
  }

  return interval;
}

////////////////////////////////////////////////////////////////////////////////
