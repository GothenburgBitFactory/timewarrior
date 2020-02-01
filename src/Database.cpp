////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <cassert>
#include <Database.h>
#include <format.h>
#include <JSON.h>
#include <IntervalFactory.h>
#include <iostream>
#include <iomanip>
#include <shared.h>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
Database::iterator::iterator (files_iterator fbegin, files_iterator fend) :
          files_it(fbegin),
          files_end(fend)
{
    if (files_end != files_it)
    {
      auto &lines = files_it->allLines ();
      lines_it = lines.rbegin ();
      lines_end = lines.rend ();
      while ((lines_it == lines_end) && (files_it != files_end))
      {
        ++files_it;
        if (files_it != files_end)
        {
          auto& lines = files_it->allLines ();
          lines_it = lines.rbegin ();
          lines_end = lines.rend ();
        }
      }
    }
}

////////////////////////////////////////////////////////////////////////////////
Database::iterator& Database::iterator::operator++()
{
  if (files_it != files_end)
  {
    if (lines_it != lines_end)
    {
      ++lines_it;

      // If we are at the end of the current file, we will need to advance to
      // the next file here. A file may be empty, which is why we need to wait
      // until we are pointing at a valid line.
      while ((lines_it == lines_end) && (files_it != files_end))
      {
        ++files_it;
        if (files_it != files_end)
        {
          auto& lines = files_it->allLines ();
          lines_it = lines.rbegin ();
          lines_end = lines.rend ();
        }
      }
    }
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool Database::iterator::operator==(const iterator & other) const
{
  return (other.files_it == other.files_end) ?
          files_it == files_end :
         ((files_it == other.files_it) &&
          (files_end == other.files_end) &&
          (lines_it == other.lines_it) &&
          (lines_end == other.lines_end));
}

////////////////////////////////////////////////////////////////////////////////
bool Database::iterator::operator!=(const iterator & other) const
{
  return ! (*this == other);
}

////////////////////////////////////////////////////////////////////////////////
const std::string& Database::iterator::operator*() const
{
  assert(lines_it != lines_end);
  return *lines_it;
}

////////////////////////////////////////////////////////////////////////////////
const std::string* Database::iterator::operator->() const
{
  assert(lines_it != lines_end);
  return &(*lines_it);
}

////////////////////////////////////////////////////////////////////////////////
Database::reverse_iterator::reverse_iterator (files_iterator fbegin,
                                              files_iterator fend) :
          files_it(fbegin),
          files_end(fend)
{
    if (files_end != files_it)
    {
      lines_it = files_it->allLines ().begin ();
      lines_end = files_it->allLines ().end ();
      while ((lines_it == lines_end) && (files_it != files_end))
      {
        ++files_it;
        if (files_it != files_end)
        {
          auto& lines = files_it->allLines ();
          lines_it = lines.begin ();
          lines_end = lines.end ();
        }
      }
    }
}

////////////////////////////////////////////////////////////////////////////////
Database::reverse_iterator& Database::reverse_iterator::operator++()
{
  if (files_it != files_end)
  {
    if (lines_it != lines_end)
    {
      ++lines_it;

      // If we are at the end of the current file, we will need to advance to
      // the next file here. A file may be empty, which is why we need to wait
      // until we are pointing at a valid line.
      while ((lines_it == lines_end) && (files_it != files_end))
      {
        ++files_it;
        if (files_it != files_end)
        {
          lines_it = files_it->allLines ().begin ();
          lines_end = files_it->allLines ().end ();
        }
      }
    }
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////
bool
Database::reverse_iterator::operator==(const reverse_iterator & other) const
{
  return (other.files_it == other.files_end) ?
          files_it == files_end :
         ((files_it == other.files_it) &&
          (files_end == other.files_end) &&
          (lines_it == other.lines_it) &&
          (lines_end == other.lines_end));
}

////////////////////////////////////////////////////////////////////////////////
bool
Database::reverse_iterator::operator!=(const reverse_iterator & other) const
{
  return ! (*this == other);
}

////////////////////////////////////////////////////////////////////////////////
const std::string& Database::reverse_iterator::operator*() const
{
  assert (lines_it != lines_end);
  return *lines_it;
}

////////////////////////////////////////////////////////////////////////////////
const std::string* Database::reverse_iterator::operator->() const
{
  return &operator*();
}

////////////////////////////////////////////////////////////////////////////////
Database::iterator Database::begin ()
{
  if (_files.empty ())
  {
    initializeDatafiles ();
  }

  return iterator (_files.rbegin (), _files.rend ());
}

////////////////////////////////////////////////////////////////////////////////
Database::iterator Database::end ()
{
  if (_files.empty ())
  {
    initializeDatafiles ();
  }

  return iterator (_files.rend (), _files.rend ());
}


////////////////////////////////////////////////////////////////////////////////
Database::reverse_iterator Database::rbegin ()
{
  if (_files.empty ())
  {
    initializeDatafiles ();
  }

  return reverse_iterator(_files.begin (), _files.end ());
}

////////////////////////////////////////////////////////////////////////////////
Database::reverse_iterator Database::rend ()
{
  if (_files.empty ())
  {
    initializeDatafiles ();
  }

  return reverse_iterator (_files.end (), _files.end ());
}

////////////////////////////////////////////////////////////////////////////////
void Database::initialize (const std::string& location, Journal& journal)
{
  _location = location;
  _journal = &journal;
  initializeTagDatabase ();
}

////////////////////////////////////////////////////////////////////////////////
void Database::commit ()
{
  for (auto& file : _files)
  {
    file.commit ();
  }

  File::write (_location + "/tags.data", _tagInfoDatabase.toJson ());
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Database::files () const
{
  std::vector <std::string> all;
  for (auto& file : _files)
  {
    all.push_back (file.name ());
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::set <std::string> Database::tags () const
{
  return _tagInfoDatabase.tags ();
}

////////////////////////////////////////////////////////////////////////////////
// Return most recent line from database 
std::string Database::getLatestEntry ()
{
  for (auto& line : *this)
  {
    if (! line.empty ())
    {
      return line;
    }
  }

  return "";
}

////////////////////////////////////////////////////////////////////////////////
void Database::addInterval (const Interval& interval, bool verbose)
{
  assert ( (interval.end == 0) || (interval.start <= interval.end));

  auto tags = interval.tags ();
  for (auto& tag : tags)
  {
    if (_tagInfoDatabase.incrementTag (tag) == -1 && verbose)
    {
      std::cout << "Note: '" << quoteIfNeeded (tag) << "' is a new tag." << std::endl;
    }
  }

  // Get the index into _files for the appropriate Datafile, which may be
  // created on demand.
  auto df = getDatafile (interval.start.year (), interval.start.month ());
  _files[df].addInterval (interval);
  _journal->recordIntervalAction ("", interval.json ());
}

void Database::deleteInterval (const Interval& interval)
{
  auto tags = interval.tags ();

  for (auto& tag : tags)
  {
    _tagInfoDatabase.decrementTag (tag);
  }

  // Get the index into _files for the appropriate Datafile, which may be
  // created on demand.
  auto df = getDatafile (interval.start.year (), interval.start.month ());

  _files[df].deleteInterval (interval);
  _journal->recordIntervalAction (interval.json (), "");
}

////////////////////////////////////////////////////////////////////////////////
// The algorithm to modify an interval is first to find and remove it from the
// Datafile, then add it back to the right Datafile. This is because
// modification may involve changing the start date, which could mean the
// Interval belongs in a different file.
void Database::modifyInterval (const Interval& from, const Interval& to, bool verbose)
{
  if (!from.empty ())
  {
    deleteInterval (from);
  }

  if (!to.empty ())
  {
    addInterval (to, verbose);
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string Database::dump () const
{
  std::stringstream out;
  out << "Database\n";
  for (auto& df : _files)
  {
    out << df.dump ();
  }

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
  auto basename = File (name).name ();

  // If the datafile is already initialized, return.
  for (unsigned int i = 0; i < _files.size (); ++i)
  {
    if (_files[i].name () == basename)
    {
      return i;
    }
  }

  // Create the Datafile.
  Datafile df;
  df.initialize (name);

  // Insert Datafile into _files. The position is not important.
  _files.push_back (df);
  return _files.size () - 1;
}

////////////////////////////////////////////////////////////////////////////////
// The input Daterange has a start and end, for example:
//
//   2016-02-20 to 2016-04-15
//
// Given the monthly storage scheme, split the Daterange into a vector of
// segmented Dateranges:
//
//   2016-02-01 to 2016-03-01
//   2016-03-01 to 2016-04-01
//   2016-04-01 to 2016-05-01
//
std::vector <Range> Database::segmentRange (const Range& range)
{
  std::vector <Range> segments;

  auto start_y = range.start.year ();
  auto start_m = range.start.month ();

  auto end = range.end;
  if (end.toEpoch () == 0)
  {
    end = Datetime ();
  }

  auto end_y = end.year ();
  auto end_m = end.month ();

  while (start_y < end_y ||
         (start_y == end_y && start_m <= end_m))
  {
    // Capture date before incrementing month.
    Datetime segmentStart (start_y, start_m, 1);

    // Next month.
    start_m += 1;
    if (start_m > 12)
    {
      start_y += 1;
      start_m = 1;
    }

    // Capture date after incrementing month.
    Datetime segmentEnd (start_y, start_m, 1);
    auto segment = Range (segmentStart, segmentEnd);
    if (range.intersects (segment))
    {
      segments.push_back (segment);
    }
  }

  return segments;
}

bool Database::empty ()
{
  return Database::begin () == Database::end ();
}

////////////////////////////////////////////////////////////////////////////////
void Database::initializeTagDatabase ()
{
  std::string content;

  if (!File::read (_location + "/tags.data", content))
  {
    auto it = Database::begin ();
    auto end = Database::end ();
    
    if (it == end)
    {
      return;
    }

    std::cout << "Tag info database does not exist. Recreating from interval data..." << std::endl  ;

    for (; it != end; ++it)
    {
      Interval interval = IntervalFactory::fromSerialization (*it);
      for (auto& tag : interval.tags ())
      {
        _tagInfoDatabase.incrementTag (tag);
      }
    }
  }
  else
  {
    auto *json = (json::object *) json::parse (content);

    for (auto &pair : json->_data)
    {
      auto key = str_replace (pair.first, "\\\"", "\"");
      auto *value = (json::object *) pair.second;
      auto iter = value->_data.find ("count");

      if (iter == value->_data.end ())
      {
        throw format ("Failed to find \"count\" member for tag \"{1}\" in tags database. Database corrupted?", key);
      }

      auto number = dynamic_cast<json::number *> (iter->second);
      _tagInfoDatabase.add (key, TagInfo{(unsigned int) number->_dvalue});
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void Database::initializeDatafiles ()
{
  // Because the data files have names YYYY-MM.data, sorting them by name also
  // sorts by the intervals within.
  Directory d (_location);
  auto files = d.list ();
  std::sort (files.begin (), files.end ());

  for (auto& file : files)
  {
    // If it looks like a data file: *-??.data
    if (file[file.length () - 8] == '-' &&
        file.find (".data") == file.length () - 5)
    {
      auto basename = File (file).name ();
      auto year  = strtol (basename.substr (0, 4).c_str (), NULL, 10);
      auto month = strtol (basename.substr (5, 2).c_str (), NULL, 10);
      getDatafile (year, month);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
