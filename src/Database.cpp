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
  auto current = currentDataFile ();

  // Because the data files have names YYYY-MM.data, sorting them by name also
  // sorts by the intervals within.
  Directory d (_location);
  auto files = d.list ();
  std::sort (files.begin (), files.end ());

  // Ensure the list always contains the name of the current file, even if it
  // does not exist.
  if (std::find (files.begin (), files.end (), current) == files.end ())
    files.push_back (current);

  for (auto& file : files)
  {
    // If it looks like a data file.
    if (file.find (".data") == file.length () - 5)
    {
      Datafile df;
      df.initialize (file);

      // New files need the set of current exclusions.
      if (! File (file).exists ())
        df.setExclusions (_exclusions);

      _files.push_back (df);
    }
  }
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
void Database::addInterval (const Interval& interval)
{
  std::vector <Datafile>::reverse_iterator ri;
  for (ri = _files.rbegin (); ri != _files.rend (); ri++)
    if (ri->addInterval (interval))
      return;

  // Datafile for this interval does not exist. This means the data file was
  // deleted/removed, or the interval is old. Create the file.
  createNewDatafile (interval.start ().year (), interval.start ().month ());
}

////////////////////////////////////////////////////////////////////////////////
void Database::deleteInterval (const Interval& interval)
{
  std::vector <Datafile>::reverse_iterator ri;
  for (ri = _files.rbegin (); ri != _files.rend (); ri++)
    if (ri->deleteInterval (interval))
      break;

  // Datafile for this interval does not exist. This means the data file was
  // deleted/removed, or the interval is old. Nothing to do.
}

////////////////////////////////////////////////////////////////////////////////
void Database::modifyInterval (const Interval& from, const Interval& to)
{
  // The algorithm to modify an interval is first to find and remove it from the
  // Datafile, then add it back to the right Datafile. This is because
  // modifїcation may involve changing the start date, which could mean the
  // Interval belongs in a different file.

  // Delete the interval, if found. If not found, then this means the data file
  // was deleted/moved.
  std::vector <Datafile>::reverse_iterator ri;
  for (ri = _files.rbegin (); ri != _files.rend (); ri++)
    if (ri->deleteInterval (from))
      break;

  // Add the interval to the right file.
  for (ri = _files.rbegin (); ri != _files.rend (); ri++)
    if (ri->addInterval (to))
      return;

  // There was no file that accepted the interval, which means the data file was
  // deleted/moved. recreate the file.
  createNewDatafile (to.start ().year (), to.start ().month ());
}

////////////////////////////////////////////////////////////////////////////////
std::string Database::dump () const
{
  std::stringstream out;
  out << "Database\n";
  for (auto& exclusion : _exclusions)
    out << "  Exclusion: " << exclusion << "\n";

  out << "Datafiles\n";
  for (auto& df : _files)
    out << df.dump ();

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::string Database::currentDataFile () const
{
  time_t current;
  time (&current);
  struct tm* t = gmtime (&current);

  std::stringstream out;
  out << _location
      << '/'
      << std::setw (4) << std::setfill ('0') << (t->tm_year + 1900)
      << '-'
      << std::setw (2) << std::setfill ('0') << (t->tm_mon + 1)
      << ".data";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
void Database::createNewDatafile (int year, int month)
{
  std::stringstream file;
  file << _location
       << '/'
       << std::setw (4) << std::setfill ('0') << year
       << '-'
       << std::setw (2) << std::setfill ('0') << month
       << ".data";

  // Create the Datafile. New files need the set of current exclusions.
  Datafile df;
  df.initialize (file.str ());
  df.setExclusions (_exclusions);

  // Insert Datafile into _files. The position is not important.
  _files.push_back (df);
}

////////////////////////////////////////////////////////////////////////////////
