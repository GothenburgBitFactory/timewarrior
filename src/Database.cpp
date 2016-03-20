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
#include <sstream>
#include <iomanip>
#include <ctime>

////////////////////////////////////////////////////////////////////////////////
void Database::initialize (const std::string& location)
{
  _location = location;

  // _data_files[0] is always the current file.
  _current = currentDataFile ();
  Datafile currentFile;
  currentFile.initialize (_current);
  _files.push_back (currentFile);

  Directory d (_location);
  for (const auto& file : d.list ())
  {
    if (1 /* TODO looks like one of our data files */)
    {
      if (file != _current)
      {
        Datafile oldFile;
        oldFile.initialize (file);
        _files.push_back (oldFile);
      }
    }
  }

  // TODO If there is no data file named YYYY—MM.data, then create it.
}

////////////////////////////////////////////////////////////////////////////////
std::string Database::dump () const
{
  std::stringstream out;
  out << "Database\n";
  for (const auto& file : _files)
    out << "  Datafile: " << file.name ()
        << (file.name () == _current ? " (current)" : "")
        << "\n";

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
