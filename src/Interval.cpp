////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <JSON.h>

////////////////////////////////////////////////////////////////////////////////
bool Interval::empty () const
{
  return start.toEpoch () == 0 &&
         end.toEpoch ()   == 0 &&
    _tags.empty ();
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

  if (start.toEpoch ())
    out << " " << start.toISO ();

  if (end.toEpoch ())
    out << " - " << end.toISO ();

  if (! _tags.empty ())
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

  if (is_started ())
    out << "\"start\":\"" << start.toISO () << "\"";

  if (is_ended ())
  {
    if (is_started ())
      out << ',';
    out << "\"end\":\"" << end.toISO () << "\"";
  }

  if (! _tags.empty ())
  {
    std::string tags;
    for (auto& tag : _tags)
    {
      if (tags[0])
        tags += ',';

      tags += "\"" + escape (tag, '"') + "\"";
    }

    if (start.toEpoch () ||
        end.toEpoch ())
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

  if (start.toEpoch ())
    out << " " << start.toISOLocalExtended ();

  if (end.toEpoch ())
    out << " - " << end.toISOLocalExtended ();

  if (! _tags.empty ())
  {
    out << " #";
    for (auto& tag : _tags)
      out << ' ' << quoteIfNeeded (tag);
  }

  if (synthetic)
    out << " synthetic";

  return out.str ();
}

void Interval::setRange (const Range& range)
{
  start = range.start;
  end = range.end;
}

void Interval::setRange (const Datetime& start, const Datetime& end)
{
  this->start = start;
  this->end = end;
}


////////////////////////////////////////////////////////////////////////////////
