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
Range::Range (const Datetime& start_value, const Datetime& end_value)
{
  start = start_value;
  end = end_value;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::operator== (const Range& other) const
{
  return start == other.start &&
         end   == other.end;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::started () const
{
  return start.toEpoch () > 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::ended () const
{
  return end.toEpoch () > 0;
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
  if (! started () || ! other.started ())
    return false;

  // Other range ends before this range starts.
  if (other.ended () && other.end <= start)
    return false;

  // Other range starts after this range ends.
  if (ended () && other.start >= end)
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
  if (overlap (other))
  {
    // Intersection is choosing the later of the two starts, and the earlier of
    // the two ends, provided the two ranges overlap.
    Range result;
    result.start = start > other.start ? start : other.start;

    if (ended ())
    {
      if (other.ended ())
        result.end  = end < other.end ? end : other.end;
      else
        result.end = end;
    }
    else
    {
      if (other.ended ())
        result.end = other.end;
    }

    return result;
  }

  return Range (Datetime (0), Datetime (0));
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
    if (start < other.start)
    {
      results.push_back (Range (start, other.start));

      if (other.ended () &&
          (! ended () || end > other.end))
      {
        results.push_back (Range (other.end, end));
      }
    }
    else
    {
      if (other.ended ())
      {
        if (ended ())
        {
          if (end > other.end)
            results.push_back (Range (other.end, end));
        }
        else
        {
          results.push_back (Range (other.end, end));
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
      << (start.toEpoch () ? start.toISOLocalExtended () : "n/a")
      << " - "
      << (end.toEpoch () ? end.toISOLocalExtended () : "n/a");

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
