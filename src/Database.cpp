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
#include <Database.h>
#include <FS.h>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iomanip>
#include <ctime>

////////////////////////////////////////////////////////////////////////////////
void Database::initialize (const std::string& location)
{
  _location = location;

  // Because the data files have names YYYY-MM.data, sorting them by name also
  // sorts by the intervals within.
  Directory d (_location);
  auto files = d.list ();
  std::sort (files.begin (), files.end ());
}

////////////////////////////////////////////////////////////////////////////////
void Database::commit ()
{
  for (auto& file : _files)
    file.commit ();
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Database::files () const
{
  std::vector <std::string> all;
  for (auto& file : _files)
    all.push_back (file.name ());

  return all;
}

////////////////////////////////////////////////////////////////////////////////
// Walk backwards through the files until an interval is found.
std::string Database::lastLine ()
{
  std::vector <Datafile>::reverse_iterator ri;
  for (ri = _files.rbegin (); ri != _files.rend (); ri++)
    return ri->lastLine ();

  return "";
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Database::allLines ()
{
  std::vector <std::string> all;
  for (auto& file : _files)
  {
    auto i = file.allLines ();
    all.insert (all.end (),
                std::make_move_iterator (i.begin ()),
                std::make_move_iterator (i.end ()));
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
void Database::clearExclusions ()
{
  _exclusions.clear ();
}

////////////////////////////////////////////////////////////////////////////////
void Database::addExclusion (const std::string& exclusion)
{
  _exclusions.push_back (exclusion);
}

////////////////////////////////////////////////////////////////////////////////
// interval had a start and end, for example:
//   2016-02-20 to 2016-04-15
//
// This interval spans four months, which correspond to data files:
//   2016-02-01 to 2016-03-01
//   2016-03-01 to 2016-04-01
//   2016-04-01 to 2016-05-01
//
// Intersecting these with the original interval:
//   2016-02-20 to 2016-03-01
//   2016-03-01 to 2016-04-01
//   2016-04-01 to 2016-05-15
//
void Database::addInterval (const Interval& interval)
{
  // TODO Need to verify that interval.tags do not overlap with stored data.
  //      Unless the tags that overlap are allowed to overlap.

  auto intervalRange = interval.range ();
  for (auto& segment : segmentRange (intervalRange))
  {
    // Get the index into _files for the appropriate Datafile, which may be
    // created on demand.
    auto df = getDatafile (segment.start ().year (), segment.start ().month ());

    // Intersect the original interval range, and the segment.
    Interval segmentedInterval (interval);
    segmentedInterval.range (intervalRange.intersect (segment));
    _files[df].addInterval (segmentedInterval);
  }
}

////////////////////////////////////////////////////////////////////////////////
void Database::deleteInterval (const Interval& interval)
{
  auto intervalRange = interval.range ();
  for (auto& segment : segmentRange (intervalRange))
  {
    // Get the index into _files for the appropriate Datafile, which may be
    // created on demand.
    auto df = getDatafile (segment.start ().year (), segment.start ().month ());

    // Intersect the original interval range, and the segment.
    Interval segmentedInterval (interval);
    segmentedInterval.range (intervalRange.intersect (segment));
    _files[df].deleteInterval (segmentedInterval);
  }
}

////////////////////////////////////////////////////////////////////////////////
// The algorithm to modify an interval is first to find and remove it from the
// Datafile, then add it back to the right Datafile. This is because
// modifїcation may involve changing the start date, which could mean the
// Interval belongs in a different file.
void Database::modifyInterval (const Interval& from, const Interval& to)
{
  deleteInterval (from);
  addInterval (to);
}

////////////////////////////////////////////////////////////////////////////////
std::string Database::dump () const
{
  std::stringstream out;
  out << "Database\n";
  for (auto& exclusion : _exclusions)
    out << "  Exclusion: " << exclusion << "\n";

  for (auto& df : _files)
    out << df.dump ();

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
unsigned int Database::getDatafile (int year, int month)
{
  std::stringstream file;
  file << _location
       << '/'
       << std::setw (4) << std::setfill ('0') << year
       << '-'
       << std::setw (2) << std::setfill ('0') << month
       << ".data";
  auto name = file.str ();

  // If the datafile is already initialized, return.
  for (unsigned int i = 0; i < _files.size (); ++i)
    if (_files[i].name () == name)
      return i;

  // Create the Datafile. New files need the set of current exclusions.
  Datafile df;
  df.initialize (name);
  df.setExclusions (_exclusions);

  // Insert Datafile into _files. The position is not important.
  _files.push_back (df);
  return _files.size () - 1;
}

////////////////////////////////////////////////////////////////////////////////
// The input Datarange has a start and end, for example:
//
//   2016-01-20 to 2016-04-15
//
// Given the monthly storage scheme, split the Datarange into a vector of
// segmented Dataranges:
//
//   2016-01-20 to 2016-02-01
//   2016-02-01 to 2016-03-01
//   2016-03-01 to 2016-04-01
//   2016-04-01 to 2016-05-15
//
std::vector <Daterange> Database::segmentRange (const Daterange& range)
{
  std::vector <Daterange> segments;

  auto start_y = range.start ().year ();
  auto start_m = range.start ().month ();

  auto end = range.end ();
  if (end.toEpoch () == 0)
    end = Datetime ();

  auto end_y = end.year ();
  auto end_m = end.month ();

  while (start_y < end_y ||
         (start_y == end_y && start_m <= end_m))
  {
    // Capture date before incrementing month.
    Datetime segmentStart (start_m, 1, start_y);

    // Next month.
    start_m += 1;
    if (start_m > 12)
    {
      start_y += 1;
      start_m = 1;
    }

    // Capture date after incrementing month.
    Datetime segmentEnd (start_m, 1, start_y);
    segments.push_back (Daterange (segmentStart, segmentEnd));
  }

  return segments;
}

////////////////////////////////////////////////////////////////////////////////
