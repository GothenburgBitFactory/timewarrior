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
#include <Range.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
// A Range consists of a start time and optional end time. A missing end
// time makes the Range 'started' but not 'ended'.
//
//   [start, end)
//
Range::Range (const Datetime& start, const Datetime& end)
{
  _start = start;
  _end = end;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::operator== (const Range& other) const
{
  return _start == other._start &&
         _end   == other._end;
}

////////////////////////////////////////////////////////////////////////////////
Datetime Range::start () const
{
  return _start;
}

////////////////////////////////////////////////////////////////////////////////
void Range::start (const Datetime& value)
{
  _start = value;
}

////////////////////////////////////////////////////////////////////////////////
Datetime Range::end () const
{
  return _end;
}

////////////////////////////////////////////////////////////////////////////////
void Range::end (const Datetime& value)
{
  _end = value;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::isStarted () const
{
  return _start.toEpoch () > 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::isEnded () const
{
  return _end.toEpoch   () > 0;
}

////////////////////////////////////////////////////////////////////////////////
// Detect the following overlap cases:
//
// this                     |--------|
//   A        |--------|
//   B                 |--------|
//   C                        |----|
//   D                           |--------|
//   E                                    |--------|
//   F                    |-------------|
//   G                    |...
//   H                         |...
//   I                                 |...
//
// this                     |...
//   A        |--------|
//   B                 |--------|
//   C                        |----|
//   D                           |--------|
//   E                                    |--------|
//   F                    |-------------|
//   G                    |...
//   H                         |...
//   I                                 |...
//
bool Range::overlap (const Range& other) const
{
  if (! isStarted () ||
      ! other.isStarted ())
    return false;

  // Other range ends before this range starts.
  if (other.isEnded () &&
      other.end () < start ())
    return false;

  // Other range starts after this range ends.
  if (isEnded () &&
      other.start () >= end ())
    return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////
// Calculate the following intersection cases:
//
// this                     |--------|
//   A        |--------|
//   B                 |--------|
//   C                        |----|
//   D                           |--------|
//   E                                    |--------|
//   F                    |-------------|
//   G                    |...
//   H                         |...
//   I                                 |...
//
// this                     |...
//   A        |--------|
//   B                 |--------|
//   C                        |----|
//   D                           |--------|
//   E                                    |--------|
//   F                    |-------------|
//   G                    |...
//   H                         |...
//   I                                 |...
//
Range Range::intersect (const Range& other) const
{
  Range result;

  if (overlap (other))
  {
    // Intersection is choosing the later of the two starts, and the earlier of
    // the two ends, of two overlapping ranges.
    result.start (start () > other.start () ? start () : other.start ());

    if (isEnded ())
    {
      if (other.isEnded ())
        result.end (end () < other.end () ? end () : other.end ());
      else
        result.end (end ());
    }
    else
    {
      if (other.isEnded ())
        result.end (other.end ());
    }
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Consider the following overlap cases:
//
// this                     |--------|
//   A        |--------|
//   B                 |--------|
//   C                        |----|
//   D                           |--------|
//   E                                    |--------|
//   F                    |-------------|
//   G                    |...
//   H                         |...
//   I                                 |...
//
// this                     |...
//   A        |--------|
//   B                 |--------|
//   C                        |----|
//   D                           |--------|
//   E                                    |--------|
//   F                    |-------------|
//   G                    |...
//   H                         |...
//   I                                 |...
//
std::vector <Range> Range::subtract (const Range& other) const
{
  std::vector <Range> results;

  if (overlap (other))
  {
    if (start () < other.start ())
    {
      results.push_back (Range (start (), other.start ()));

      if (other.isEnded () &&
          (! isEnded () || end () > other.end ()))
      {
        results.push_back (Range (other.end (), end ()));
      }
    }
    else
    {
      if (other.isEnded ())
      {
        if (isEnded ())
        {
          if (end () > other.end ())
            results.push_back (Range (other.end (), end ()));
        }
        else
        {
          results.push_back (Range (other.end (), end ()));
        }
      }
    }
  }

  // Non-overlapping subtraction is a nop.
  else
  {
    results.push_back (*this);
  }

  return results;
}

////////////////////////////////////////////////////////////////////////////////
std::string Range::dump () const
{
  std::stringstream out;
  out << "Range "
      << (_start.toEpoch () ? _start.toISOLocalExtended () : "n/a")
      << " - "
      << (_end.toEpoch () ? _end.toISOLocalExtended () : "n/a");

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
