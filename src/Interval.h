////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_INTERVAL
#define INCLUDED_INTERVAL

#include <Range.h>
#include <set>
#include <string>
#include <utility>

class Interval : public Range
{
public:
  Interval () = default;
  Interval (const Datetime& start, const Datetime& end) : Range (start, end) {}
  Interval (const Range& range, std::set <std::string> tags) : Range (range), _tags(std::move(tags)) {}

  bool operator== (const Interval&) const;
  bool operator!= (const Interval&) const;

  bool empty () const;
  bool hasTag (const std::string&) const;
  const std::set <std::string>& tags () const;
  void tag (const std::string&);
  void tag (const std::set <std::string>&);
  void untag (const std::string&);
  void untag (const std::set <std::string>&);
  void clearTags ();

  void setRange (const Range& range);
  void setRange (const Datetime& start, const Datetime& end);

  void setAnnotation(const std::string& annotation);
  std::string getAnnotation() const;

  std::string serialize () const;
  std::string json () const;
  std::string dump () const;

public:
  int                    id        {0};
  bool                   synthetic {false};
  std::string            annotation {};

private:
  std::set <std::string> _tags  {};
};

#endif
