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
#include <Interval.h>
#include <timew.h>
#include <format.h>
#include <Duration.h>
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
      _start = Datetime (tokens[1]);
      offset = 1;

      // Optional '-' <iso>
      if (tokens.size () > 3 &&
          tokens[2] == "-"   &&
          tokens[3].length () == 16)
      {
        _end = Datetime (tokens[3]);
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
Datetime Interval::start () const
{
  return _start;
}

////////////////////////////////////////////////////////////////////////////////
void Interval::start (Datetime value)
{
  _start = value;
}

////////////////////////////////////////////////////////////////////////////////
Datetime Interval::end () const
{
  return _end;
}

////////////////////////////////////////////////////////////////////////////////
void Interval::end (Datetime value)
{
  _end = value;
}

////////////////////////////////////////////////////////////////////////////////
bool Interval::isStarted () const
{
  return _start.toEpoch () > 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Interval::isEnded () const
{
  return _end.toEpoch   () > 0;
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
std::string Interval::summarize () const
{
  std::stringstream out;

  if (isStarted ())
  {
    if (isEnded ())
    {
      Duration dur (Datetime (_end) - Datetime (_start));
      out << "Recorded interval from "
          << _start.toISOLocalExtended ()
          << " to "
          << _end.toISOLocalExtended ()
          << ", length "
          << dur.format ();
    }
    else
    {
      Duration dur (Datetime () - _start);
      out << "Active interval since "
          << _start.toISOLocalExtended ();

      if (dur.toTime_t () > 10)
        out << ", length "
            << dur.format ();
    }

    // TODO Colorize tags.
    if (_tags.size ())
    {
      out << ", using tags:";
      for (auto& tag : _tags)
        out << ' ' << quoteIfNeeded (tag);
    }

    out << "\n";
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Interval::serialize () const
{
  std::stringstream out;
  out << "inc";

  if (_start.toEpoch ())
    out << " " << _start.toISO ();

  if (_end.toEpoch ())
    out << " - " << _end.toISO ();

  if (_tags.size ())
  {
    out << " #";
    for (auto& tag : _tags)
      out << ' ' << quoteIfNeeded (tag);
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Interval::dump () const
{
  std::stringstream out;

  out << "Interval _start '"
      << _start.toEpoch ()
      << "' _end '"
      << _end.toEpoch ()
      << "' _tags";

  for (const auto& tag : _tags)
    out << " '" << tag << "'";

  out << "\n";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
