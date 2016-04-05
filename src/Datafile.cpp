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
#include <Datafile.h>
#include <format.h>
#include <sstream>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
void Datafile::initialize (const std::string& name)
{
  _file = File (name);

  // From the name, which is of the form YYYY-MM.data, extract the YYYY and MM.
  auto basename = _file.name ();
  auto year  = strtol (basename.substr (0, 4).c_str (), NULL, 10);
  auto month = strtol (basename.substr (5, 2).c_str (), NULL, 10);

  // Construct two dates that
  _day1 = Datetime (month, 1,                                   year,  0 , 0,  0);
  _dayN = Datetime (month, Datetime::daysInMonth (year, month), year, 23, 59, 59);
}

////////////////////////////////////////////////////////////////////////////////
std::string Datafile::name () const
{
  return _file.name ();
}

////////////////////////////////////////////////////////////////////////////////
Interval Datafile::getLatestInterval ()
{
  // Load data
  if (! _intervals_loaded)
    load_intervals ();

  // Return the last element in _lines.
  if (_intervals.size ())
    return _intervals.back ();

  return Interval ();
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> Datafile::getAllIntervals ()
{
  if (! _intervals_loaded)
    load_intervals ();

  return _intervals;
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::addExclusion (const std::string& exclusion)
{
  _dirty = true;
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::addInterval (const Interval& interval)
{
  _lines_added.push_back (interval.serialize ());
  _dirty = true;
}

////////////////////////////////////////////////////////////////////////////////
// Only checks if the start time matches.
void Datafile::modifyInterval (const Interval& interval)
{
  for (auto& i : _intervals)
  {
    if (i.start () == interval.start ())
    {
      i = interval;
      _dirty = true;
      _lines_modified = true;
      break;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::commit ()
{
  // The _dirty flag indicates that the file needs to be written.
  if (_dirty)
  {
    // Special case: added but no modified means just append to the file.
    if (_lines_added.size () &&
        ! _lines_modified)
    {
      if (_file.open ())
      {
        _file.lock ();

        // Write out all the added intervals.
        _file.append (std::string(""));  // Seek to end of file

        // Write out all the added lines.
        for (auto& line : _lines_added)
          _file.write_raw (line + "\n");

        _lines_added.clear ();
        _file.close ();

        _dirty = false;
      }
      else
        throw format ("Could not write to data file {1}", _file._data);
    }
    else
    {
      if (_file.open ())
      {
        _file.lock ();

        // Truncate the file and rewrite.
        _file.truncate ();

        // Rewrite all the intervals, because at least one was modified.
        _file.append (std::string(""));  // Seek to end of file
        for (auto& interval : _intervals)
          _file.write_raw (interval.serialize () + "\n");

        // Write out all the added lines.
        for (auto& line : _lines_added)
          _file.write_raw (line + "\n");

        _lines_added.clear ();
        _file.close ();
        _dirty = false;
      }
      else
        throw format ("Could not write to data file {1}", _file._data);
    }

    _dirty = false;
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string Datafile::dump () const
{
  std::stringstream out;
  out << "Datafile\n"
      << "  Name: " << _file.name () << "\n"
      << "  day1: " << _day1.toISO () << "\n"
      << "  dayN: " << _dayN.toISO () << "\n";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::load_intervals ()
{
  if (! _lines_loaded)
  {
    load_lines ();

    // Apply previously added lines.
    for (auto& line : _lines_added)
      _lines.push_back (line);
  }

  for (auto& line : _lines)
  {
    // "inc ..."
    if (line[0] == 'i')
    {
      Interval i;
      i.initialize (line);
      _intervals.push_back (i);
    }

    // "exc ..."
    else if (line[0] == 'e')
      ;  // TODO Exclusions.
    else
    {
      // TODO Ignore blank lines?
    }
  }

  _intervals_loaded = true;
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::load_lines ()
{
  if (_file.open ())
  {
    _file.lock ();
    _file.read (_lines);
    _file.close ();

    _lines_loaded = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
