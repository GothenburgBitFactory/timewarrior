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
#include <sstream>

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
      out << " " << tag;
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
