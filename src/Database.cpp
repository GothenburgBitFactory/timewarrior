////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <Database.h>
#include <format.h>
#include <iterator>
#include <iomanip>
#include <TransactionsFactory.h>
#include <JSON.h>
#include <iostream>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
void Database::initialize (const std::string& location)
{
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

  File::write (_location + "/tags.data", _tagInfoDatabase.toJson ());
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
  if (_files.empty ())
  {
    initializeDatafiles ();
  }

  std::vector <Datafile>::reverse_iterator ri;
  for (ri = _files.rbegin (); ri != _files.rend (); ri++)
  {
    auto line = ri->lastLine ();
    if (! line.empty ())
      return line;
  }

  return "";
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Database::allLines ()
{
  if (_files.empty ())
  {
    initializeDatafiles ();
  }

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
void Database::addInterval (const Interval& interval, bool verbose)
{
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
  auto df = getDatafile (interval.range.start.year (), interval.range.start.month ());
  _files[df].addInterval (interval);
  recordIntervalAction ("", interval.json ());
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
  auto df = getDatafile (interval.range.start.year (), interval.range.start.month ());

  _files[df].deleteInterval (interval);

  recordIntervalAction (interval.json (), "");
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
void Database::startTransaction ()
{
  if (_currentTransaction != nullptr)
  {
    throw "Subsequent call to start transaction";
  }

  _currentTransaction = std::make_shared <Transaction> ();
}

////////////////////////////////////////////////////////////////////////////////
void Database::endTransaction ()
{
  if (_currentTransaction == nullptr)
  {
    throw "Call to end non-existent transaction";
  }

  File undo (_location + "/undo.data");

  if (undo.open ())
  {
    undo.append (_currentTransaction->toString());

    undo.close ();
    _currentTransaction.reset ();
  }
  else
  {
    throw format ("Unable to write the undo transaction to {1}", undo._data);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Record undoable actions. There are several types:
//   interval    changes to stored intervals
//   config      changes to configuration
//
// Actions are only recorded if a transaction is open
//
void Database::recordUndoAction (
  const std::string &type,
  const std::string &before,
  const std::string &after)
{
  if (_currentTransaction == nullptr)
  {
    return;
  }

  _currentTransaction->addUndoAction (type, before, after);
}

////////////////////////////////////////////////////////////////////////////////
void Database::recordConfigAction (
  const std::string& before,
  const std::string& after)
{
  recordUndoAction ("config", before, after);
}

////////////////////////////////////////////////////////////////////////////////
void Database::recordIntervalAction (
  const std::string& before,
  const std::string& after)
{
  recordUndoAction ("interval", before, after);
}

////////////////////////////////////////////////////////////////////////////////
Transaction Database::popLastTransaction ()
{
  File undo (_location + "/undo.data");

  std::vector <std::string> read_lines;
  undo.read (read_lines);
  undo.close ();

  TransactionsFactory transactionsFactory;

  for (auto& line: read_lines)
  {
    transactionsFactory.parseLine (line);
  }

  std::vector <Transaction> transactions = transactionsFactory.get ();

  if (transactions.empty ())
  {
    return Transaction {};
  }

  Transaction last = transactions.back ();
  transactions.pop_back ();

  File::remove (undo._data);
  undo.open ();

  for (auto& transaction : transactions)
  {
    undo.append (transaction.toString ());
  }

  undo.close ();

  return last;
}

////////////////////////////////////////////////////////////////////////////////
std::string Database::dump () const
{
  std::stringstream out;
  out << "Database\n";
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
  auto basename = File (name).name ();

  // If the datafile is already initialized, return.
  for (unsigned int i = 0; i < _files.size (); ++i)
    if (_files[i].name () == basename)
      return i;

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
    end = Datetime ();

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
    if (range.intersects (segment)) {
      segments.push_back (segment);
    }
  }

  return segments;
}

////////////////////////////////////////////////////////////////////////////////
void Database::initializeTagDatabase ()
{
  std::string content;

  if (!File::read (_location + "/tags.data", content))
  {
    return;
  }

  auto* json = (json::object*) json::parse (content);
  
  for (auto& pair : json->_data)
  {
    auto key = pair.first;
    auto* value = (json::object*) pair.second;
    auto* number = (json::number*) value->_data["count"];

    _tagInfoDatabase.add (key, TagInfo {(unsigned int) number->_dvalue});
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
