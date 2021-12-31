////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <Datafile.h>
#include <timew.h>
#include <format.h>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <stdlib.h>
#include <AtomicFile.h>
#include <IntervalFactory.h>

////////////////////////////////////////////////////////////////////////////////
Datafile::Datafile (const std::string& name) : _file (name)
{
  // From the name, which is of the form YYYY-MM.data, extract the YYYY and MM.
  auto basename = _file.name ();
  auto year  = strtol (basename.substr (0, 4).c_str (), NULL, 10);
  auto month = strtol (basename.substr (5, 2).c_str (), NULL, 10);

  // The range is a month: [start, end).
  Datetime start (year, month, 1, 0, 0, 0);
  month++;
  if (month > 12)
  {
    year++;
    month = 1;
  }
  Datetime end (year, month, 1, 0, 0, 0);
  _range = Range (start, end);
}

////////////////////////////////////////////////////////////////////////////////
std::string Datafile::name () const
{
  return _file.name ();
}

////////////////////////////////////////////////////////////////////////////////
const Range& Datafile::range () const
{
  return _range;
}

////////////////////////////////////////////////////////////////////////////////
// Identifies the last incluѕion (^i) lines
std::string Datafile::lastLine ()
{
  if (! _lines_loaded)
    load_lines ();

  std::vector <std::string>::reverse_iterator ri;
  for (ri = _lines.rbegin (); ri != _lines.rend (); ri++)
    if (ri->operator[] (0) == 'i')
      return *ri;

  return "";
}

////////////////////////////////////////////////////////////////////////////////
const std::vector <std::string>& Datafile::allLines ()
{
  if (! _lines_loaded)
    load_lines ();

  return _lines;
}

////////////////////////////////////////////////////////////////////////////////
// Accepted intervals;   day1 <= interval.start < dayN
// Returns true if the interval was added to the datafile
bool Datafile::addInterval (const Interval& interval)
{
  bool lineAdded = false;

  // Note: end date might be zero.
  assert (interval.startsWithin (_range));

  if (! _lines_loaded)
    load_lines ();

  const std::string serialization = interval.serialize ();

  // Ensure that the IntervalFactory can properly parse the serialization before
  // adding it to the database.
  try
  {
    Interval test = IntervalFactory::fromSerialization (serialization);
    test.id = interval.id;
    if (interval != test)
    {
      throw (format ("Encode / decode check failed:\n  {1}\nis not equal to:\n  {2}",
                     interval.dump (), test.dump ()));
    }

    auto it = std::lower_bound (_lines.begin (), _lines.end (), serialization);
    if ((it != _lines.end ()) && (*it == serialization))
    {
      debug (format ("{1}: already contained {2}", _file.name (), serialization));
    }
    else
    {
      _lines.insert (it, serialization);
      debug (format ("{1}: Added {2}", _file.name (), serialization));
      _dirty = true;
      lineAdded = true;
    }
  }
  catch (const std::string& error)
  {
    debug (format ("Datafile::addInterval() failed.\n{1}", error));
    throw std::string ("Internal error. Failed encode / decode check.");
  }

  return lineAdded;
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::deleteInterval (const Interval& interval)
{
  // Note: end date might be zero.
  assert (interval.startsWithin (_range));

  if (! _lines_loaded)
  {
    load_lines ();
  }

  auto serialized = interval.serialize ();
  auto i = std::find (_lines.begin (), _lines.end (), serialized);
  if (i == _lines.end ())
  {
    throw format ("Datafile::deleteInterval failed to find '{1}'", serialized);
  }

  _lines.erase (i);
  _dirty = true;
  debug (format ("{1}: Deleted {2}", _file.name (), serialized));
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::commit ()
{
  // The _dirty flag indicates that the file needs to be written.
  if (_dirty)
  {
    AtomicFile file (_file);
    if (_lines.size () > 0)
    {
      if (file.open ())
      {
        // Write out all the lines.
        file.truncate ();
        for (auto& line : _lines)
        {
          file.write_raw (line + '\n');
        }

        _dirty = false;
      }
      else
      {
        throw format ("Could not write to data file {1}", _file._data);
      }
    }
    else
    {
      file.remove ();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string Datafile::dump () const
{
  std::stringstream out;
  out << "Datafile\n"
      << "  Name:        " << _file.name () << (_file.exists () ? "" : " (does not exist)") << '\n'
      << "  dirty:       " << (_dirty ? "true" : "false") << '\n'
      << "  lines:       " << _lines.size () << '\n'
      << "    loaded     " << (_lines_loaded ? "true" : "false") << '\n'
      << "  range:       " << _range.start.toISO () << " - "
                           << _range.end.toISO () << '\n';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::load_lines ()
{
  AtomicFile file (_file);
  if (file.open ())
  {
    // Load the data.
    file.read (_lines);
    file.close ();

    // We need the data to be sorted, sort it on load in case other tools or
    // the user hand modified the datafiles
    std::sort (_lines.begin (), _lines.end ());

    _lines_loaded = true;
    debug (format ("{1}: {2} intervals", file.name (), _lines.size ()));
  }
}

////////////////////////////////////////////////////////////////////////////////
