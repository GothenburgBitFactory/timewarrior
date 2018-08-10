////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_DATABASE
#define INCLUDED_DATABASE

#include <Datafile.h>
#include <Interval.h>
#include <Range.h>
#include <Transaction.h>
#include <vector>
#include <string>
#include <TagInfoDatabase.h>

class Database
{
public:
  Database () = default;
  void initialize (const std::string&);
  void commit ();
  std::vector <std::string> files () const;

  std::string lastLine ();
  std::vector <std::string> allLines ();

  void addInterval (const Interval&, bool verbose);
  void deleteInterval (const Interval&);
  void modifyInterval (const Interval&, const Interval &, bool verbose);

  void startTransaction ();
  void endTransaction ();
  void recordConfigAction(const std::string&, const std::string&);
  void recordIntervalAction(const std::string&, const std::string&);

  Transaction popLastTransaction ();

  std::string dump () const;

private:
  unsigned int getDatafile (int, int);
  std::vector <Range> segmentRange (const Range&);
  void initializeDatafiles ();
  void initializeTagDatabase ();

  void recordUndoAction (const std::string &, const std::string &, const std::string &);

private:
  std::string               _location {"~/.timewarrior/data"};
  std::vector <Datafile>    _files    {};
  TagInfoDatabase           _tagInfoDatabase {};
  std::shared_ptr <Transaction> _currentTransaction = nullptr;
};

#endif
