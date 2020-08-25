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

#ifndef INCLUDED_DATABASE
#define INCLUDED_DATABASE

#include <Datafile.h>
#include <Interval.h>
#include <Range.h>
#include <Transaction.h>
#include <vector>
#include <string>
#include <TagInfoDatabase.h>
#include <Journal.h>

class Database
{
public:
  using datafiles_t = std::vector <Datafile>;

  class iterator
  {
  public:
    using difference_type    = ssize_t;
    using value_type         = std::string;
    using pointer            = value_type*;
    using reference          = value_type&;
    using iterator_category  = std::forward_iterator_tag;

  private:
    friend class Database;
    typedef datafiles_t::reverse_iterator files_iterator;
    typedef std::vector <std::string>::const_reverse_iterator lines_iterator;

    files_iterator files_it;
    files_iterator files_end;

    lines_iterator lines_it;
    lines_iterator lines_end;

    iterator (files_iterator fbegin, files_iterator fend);

  public:
    iterator& operator++ ();
    bool operator== (const iterator & other) const;
    bool operator!= (const iterator & other) const;
    const value_type& operator* () const;
    const value_type* operator-> () const;
  };

  class reverse_iterator
  {
  public:
    using difference_type    = ssize_t;
    using value_type         = std::string;
    using pointer            = value_type*;
    using reference          = value_type&;
    using iterator_category  = std::forward_iterator_tag;

  private:
    friend class Database;
    typedef datafiles_t::iterator files_iterator;
    typedef std::vector <std::string>::const_iterator lines_iterator;

    files_iterator files_it;
    files_iterator files_end;

    lines_iterator lines_it;
    lines_iterator lines_end;

    reverse_iterator(files_iterator fbegin, files_iterator fend);

  public:
    reverse_iterator& operator++ ();
    bool operator== (const reverse_iterator & other) const;
    bool operator!= (const reverse_iterator & other) const;
    const value_type& operator* () const;
    const value_type* operator-> () const;
  };

public:
  Database () = delete;
  Database (const std::string&, int);
  Database (const Database&) = delete;
  Database (Database&&) = default;
  Database& operator= (const Database&) = delete;
  Database& operator= (Database&&) = default;
  void initialize (const std::string&, int);
  void commit ();
  Journal& journal ();
  std::vector <std::string> files () const;
  std::set <std::string> tags () const;

  std::string getLatestEntry ();

  void addInterval (const Interval&, bool verbose);
  void deleteInterval (const Interval&);
  void modifyInterval (const Interval&, const Interval &, bool verbose);

  std::string dump () const;

  bool empty ();
  iterator begin ();
  iterator end ();
  reverse_iterator rbegin ();
  reverse_iterator rend ();

private:
  std::vector <Range> segmentRange (const Range&);
  void initializeDatafiles () const;
  void initializeTagDatabase ();

private:
  std::string               _location;
  Journal                   _journal;
  mutable datafiles_t       _files    {};
  TagInfoDatabase           _tagInfoDatabase {};
};

#endif
