////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2022, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <Journal.h>
#include <Range.h>
#include <TagInfoDatabase.h>
#include <Transaction.h>
#include <string>
#include <vector>

class Database
{
public:

  class iterator
  {
  private:
    friend class Database;
    typedef std::vector <Datafile>::reverse_iterator files_iterator;
    typedef std::vector <std::string>::const_reverse_iterator lines_iterator;
    typedef std::string value_type;

    files_iterator files_it;
    files_iterator files_end;

    lines_iterator lines_it;
    lines_iterator lines_end;

    iterator (files_iterator fbegin, files_iterator fend);

  public:
    iterator& operator++ ();
    iterator& operator++ (int);
    iterator& operator-- ();
    bool operator== (const iterator & other) const;
    bool operator!= (const iterator & other) const;
    const value_type& operator* () const;
    const value_type* operator-> () const;
  };

  class reverse_iterator
  {
  private:
    friend class Database;
    typedef std::vector <Datafile>::iterator files_iterator;
    typedef std::vector <std::string>::const_iterator lines_iterator;
    typedef std::string value_type;

    files_iterator files_it;
    files_iterator files_end;

    lines_iterator lines_it;
    lines_iterator lines_end;

    reverse_iterator(files_iterator fbegin, files_iterator fend);

  public:
    reverse_iterator& operator++ ();
    reverse_iterator& operator++ (int);
    reverse_iterator& operator-- ();
    bool operator== (const reverse_iterator & other) const;
    bool operator!= (const reverse_iterator & other) const;
    const value_type& operator* () const;
    const value_type* operator-> () const;
  };

public:
  Database () = default;
  void initialize (const std::string&, Journal& journal);
  void commit ();
  std::vector <std::string> files () const;
  std::set <std::string> tags () const;

  std::string getLatestEntry ();

  void addInterval (const Interval&, bool verbose);
  void deleteInterval (const Interval&);
  void modifyInterval (const Interval&, const Interval&, bool verbose);

  std::string dump () const;

  bool empty ();
  iterator begin ();
  iterator end ();
  reverse_iterator rbegin ();
  reverse_iterator rend ();

private:
  unsigned int getDatafile (int, int);
  std::vector <Range> segmentRange (const Range&);
  void initializeDatafiles ();
  void initializeTagDatabase ();

private:
  std::string               _location {};
  std::vector <Datafile>    _files    {};
  TagInfoDatabase           _tagInfoDatabase {};
  Journal*                  _journal {};
};

#endif
