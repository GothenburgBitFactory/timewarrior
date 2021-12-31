////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_DATAFILE
#define INCLUDED_DATAFILE

#include <Interval.h>
#include <Range.h>
#include <FS.h>
#include <vector>
#include <string>

class Datafile
{
public:
  explicit Datafile (const std::string&);
  std::string name () const;
  const Range& range () const;

  std::string lastLine ();
  const std::vector <std::string>& allLines ();

  bool addInterval (const Interval&);
  void deleteInterval (const Interval&);
  void commit ();

  std::string dump () const;

private:
  void load_lines ();

private:
  Path                      _file         {};
  bool                      _dirty        {false};
  std::vector <std::string> _lines        {};
  bool                      _lines_loaded {false};
  Range                     _range        {};
};

#endif
