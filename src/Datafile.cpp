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
#include <algorithm>
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
  _dayN = Datetime (month, Datetime::daysInMonth (month, year), year, 23, 59, 59);
}

////////////////////////////////////////////////////////////////////////////////
std::string Datafile::name () const
{
  return _file.name ();
}

////////////////////////////////////////////////////////////////////////////////
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
std::vector <std::string> Datafile::allLines ()
{
  if (! _lines_loaded)
    load_lines ();

  return _lines;
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::setExclusions (const std::vector <std::string>& exclusions)
{
  // TODO Overwrite local copy
  // TODO load current exclusion set
  // TODO if local copy != exclusion set
  // TODO   remove old exclusion set from _lines
  // TODO   add local copy
  // TODO   _dirty = ture;

  _exclusions = exclusions;
  _dirty = true;
}

////////////////////////////////////////////////////////////////////////////////
bool Datafile::addInterval (const Interval& interval)
{
  // Return false if the interval does not belong in this file.
  if (interval.start () > _dayN ||
      interval.end ()   < _day1)
    return false;

  if (! _lines_loaded)
    load_lines ();

  // TODO if _lines contains no exclusions
  // TODO   add _exclusions

  // TODO if interval is not a duplicate
  // TODO   insert interval.serialize into _lines
  // TODO   _dirty = true;

  _lines.push_back (interval.serialize ());
  _dirty = true;
  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool Datafile::deleteInterval (const Interval& interval)
{
  // Return false if the interval does not belong in this file.
  if (interval.start () > _dayN ||
      interval.end ()   < _day1)
    return false;

  if (! _lines_loaded)
    load_lines ();

  auto serialized = interval.serialize ();
  auto i = std::find (_lines.begin (), _lines.end (), serialized);
  if (i != _lines.end ())
  {
    _lines.erase (i);
    _dirty = true;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
void Datafile::commit ()
{
  // The _dirty flag indicates that the file needs to be written.
  if (_dirty)
  {
    if (_file.open ())
    {
      _file.lock ();
      _file.truncate ();
      _file.append (std::string(""));   // Seek to EOF.

      // Write out all the lines.
      for (auto& line : _lines)
        _file.write_raw (line + "\n");

      _file.close ();
      _dirty = false;
    }
    else
      throw format ("Could not write to data file {1}", _file._data);

    _dirty = false;
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string Datafile::dump () const
{
  std::stringstream out;
  out << "Datafile\n"
      << "  Name:        " << _file.name () << (_file.exists () ? "" : " (does not exist)") << "\n"
      << "  dirty:       " << (_dirty ? "true" : "false") << "\n"
      << "  lines:       " << _lines.size () << "\n"
      << "    loaded     " << (_lines_loaded ? "true" : "false") << "\n"
      << "  exclusions:  " << _exclusions.size () << "\n"
      << "  day1:        " << _day1.toISO () << "\n"
      << "  dayN:        " << _dayN.toISO () << "\n";

  return out.str ();
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
