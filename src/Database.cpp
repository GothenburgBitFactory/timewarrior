////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <AtomicFile.h>

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
void Database::initialize (const std::string& location, int journal_size)
{
  _journal.initialize (location + "/undo.data", journal_size);
  _location = location;
  initializeTagDatabase ();
}

////////////////////////////////////////////////////////////////////////////////
void Database::commit ()
{
  for (auto& file : _files)
  {
    file.commit ();
  }

  if (_tagInfoDatabase.is_modified ())
  {
    AtomicFile::write (_location + "/tags.data", _tagInfoDatabase.toJson ());
  }
}

////////////////////////////////////////////////////////////////////////////////
Journal& Database::journal ()
{
  return _journal;
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
  if (_files[df].addInterval (interval))
  {
    _journal.recordIntervalAction ("", interval.json ());
  }
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
  _journal.recordIntervalAction (interval.json (), "");
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
  if (_files.empty ())
  {
    initializeDatafiles ();
  }

  std::stringstream out;
  out << "Database\n";
  for (auto& df : _files)
  {
    out << df.dump ();
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
unsigned int Database::getDatafile (int year, int month) const
{
  std::stringstream file;
  file << _location
       << '/'
       << std::setw (4) << std::setfill ('0') << year
       << '-'
       << std::setw (2) << std::setfill ('0') << month
       << ".data";
  auto name = file.str ();
  auto basename = Path (name).name ();

  // If the datafile is already initialized, return.
  for (unsigned int i = 0; i < _files.size (); ++i)
  {
    if (_files[i].name () == basename)
    {
      return i;
    }
  }

  // Create and insert Datafile into _files. The position is not important.
  _files.emplace_back (name);
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
  _tagInfoDatabase = TagInfoDatabase ();
  AtomicFile tags_file {_location + "/tags.data"};
  std::string content;
  const bool exists = tags_file.exists ();

  if (exists)
  {
    tags_file.read (content);
    try
    {
      std::unique_ptr <json::object> json (dynamic_cast <json::object *>(json::parse (content)));

      if (content.empty () || (json == nullptr))
      {
          throw std::string ("Contents invalid.");
      }

      for (auto &pair : json->_data)
      {
        auto key = str_replace (pair.first, "\\\"", "\"");
        auto *value = (json::object *) pair.second;
        auto iter = value->_data.find ("count");

        if (iter == value->_data.end ())
        {
          throw format ("Failed to find \"count\" member for tag \"{1}\" in tags database.", key);
        }

        auto number = dynamic_cast<json::number *> (iter->second);
        _tagInfoDatabase.add (key, TagInfo{(unsigned int) number->_dvalue});
      }

      // Since we just loaded the database from the file, there we can clear the
      // modified state so that we will not write it back out unless there is a
      // new change.
      _tagInfoDatabase.clear_modified ();

      return;
    }
    catch (const std::string& error)
    {
      std::cerr << "Error parsing tags database: " << error << '\n';
    }
  }

  // We always want the tag database file to exists.
  _tagInfoDatabase = TagInfoDatabase();
  tags_file.write_raw (_tagInfoDatabase.toJson ());

  auto it = Database::begin ();
  auto end = Database::end ();
  
  if (it == end)
  {
    return;
  }

  if (!exists)
  {
    std::cout << "Tags database does not exist. ";
  }
  
  std::cout << "Recreating from interval data..." << std::endl;

  for (; it != end; ++it)
  {
    Interval interval = IntervalFactory::fromSerialization (*it);
    for (auto& tag : interval.tags ())
    {
      _tagInfoDatabase.incrementTag (tag);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void Database::initializeDatafiles () const
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
      auto basename = Path (file).name ();
      auto year  = strtol (basename.substr (0, 4).c_str (), NULL, 10);
      auto month = strtol (basename.substr (5, 2).c_str (), NULL, 10);
      getDatafile (year, month);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
