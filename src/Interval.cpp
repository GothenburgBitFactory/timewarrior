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
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <Interval.h>
#include <timew.h>
#include <format.h>
#include <Lexer.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
// Syntax:
//   'inc' [ <iso> [ '-' <iso> ]] [ '#' <tag> [ <tag> ... ]]
void Interval::initialize (const std::string& line)
{
  Lexer lexer (line);
  std::vector <std::string> tokens;
  std::string token;
  Lexer::Type type;
  while (lexer.token (token, type))
    tokens.push_back (Lexer::dequote (token));

  // Minimal requirement 'inc'.
  if (tokens.size () &&
      tokens[0] == "inc")
  {
    unsigned int offset = 0;

    // Optional <iso>
    if (tokens.size () > 1 &&
        tokens[1].length () == 16)
    {
      range.start = Datetime (tokens[1]);
      offset = 1;

      // Optional '-' <iso>
      if (tokens.size () > 3 &&
          tokens[2] == "-"   &&
          tokens[3].length () == 16)
      {
        range.end = Datetime (tokens[3]);
        offset = 3;
      }
    }

    // Optional '#' <tag>
    if (tokens.size () > 2 + offset &&
        tokens[1 + offset] == "#")
    {
      // Optional <tag> ...
      for (unsigned int i = 2 + offset; i < tokens.size (); ++i)
        _tags.insert (tokens[i]);
    }

    return;
  }

  throw format ("Unrecognizable line '{1}'.", line);
}

////////////////////////////////////////////////////////////////////////////////
bool Interval::empty () const
{
  return range.start.toEpoch () == 0 &&
         range.end.toEpoch ()   == 0 &&
         _tags.size ()          == 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Interval::hasTag (const std::string& tag) const
{
  return std::find (_tags.begin (), _tags.end (), tag) != _tags.end ();
}

////////////////////////////////////////////////////////////////////////////////
std::set <std::string> Interval::tags () const
{
  return _tags;
}

////////////////////////////////////////////////////////////////////////////////
void Interval::tag (const std::string& tag)
{
  if (_tags.find (tag) == _tags.end ())
    _tags.insert (tag);
}

////////////////////////////////////////////////////////////////////////////////
void Interval::untag (const std::string& tag)
{
  _tags.erase (tag);
}

////////////////////////////////////////////////////////////////////////////////
std::string Interval::serialize () const
{
  std::stringstream out;
  out << "inc";

  if (range.start.toEpoch ())
    out << " " << range.start.toISO ();

  if (range.end.toEpoch ())
    out << " - " << range.end.toISO ();

  if (_tags.size ())
  {
    out << " #";
    for (auto& tag : _tags)
      out << ' ' << quoteIfNeeded (tag);
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Interval::json () const
{
  std::stringstream out;
  out << '{';

  if (range.is_started ())
    out << "\"start\":\"" << range.start.toISO () << "\"";

  if (range.is_ended ())
  {
    if (range.is_started ())
      out << ',';
    out << "\"end\":\"" << range.end.toISO () << "\"";
  }

  if (_tags.size ())
  {
    std::string tags;
    for (auto& tag : _tags)
    {
      if (tags[0])
        tags += ',';

      tags += "\"" + tag + "\"";
    }

    if (range.start.toEpoch () ||
        range.end.toEpoch ())
      out << ',';

    out << "\"tags\":["
        << tags
        << ']';
  }

  out << "}";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Interval::dump () const
{
  std::stringstream out;

  out << "interval";

  if (id)
    out << " @" << id;

  if (range.start.toEpoch ())
    out << " " << range.start.toISOLocalExtended ();

  if (range.end.toEpoch ())
    out << " - " << range.end.toISOLocalExtended ();

  if (_tags.size ())
  {
    out << " #";
    for (auto& tag : _tags)
      out << ' ' << quoteIfNeeded (tag);
  }

  if (synthetic)
    out << " synthetic";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
