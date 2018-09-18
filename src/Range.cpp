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

#include <cmake.h>
#include <Range.h>
#include <sstream>
#include <cassert>

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
bool Range::operator!= (const Range& other) const
{
  return start != other.start ||
         end   != other.end;
}

////////////////////////////////////////////////////////////////////////////////
void Range::open ()
{
  start = Datetime ();
  end = Datetime (0);
}

////////////////////////////////////////////////////////////////////////////////
void Range::open (const Datetime& value)
{
  start = value;
  end = Datetime (0);
}

////////////////////////////////////////////////////////////////////////////////
void Range::close ()
{
  end = Datetime ();
}

////////////////////////////////////////////////////////////////////////////////
bool Range::is_open () const
{
  return is_started () && ! is_ended ();
}

////////////////////////////////////////////////////////////////////////////////
bool Range::is_started () const
{
  return start.toEpoch () > 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::is_ended () const
{
  return end.toEpoch () > 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::is_empty () const
{
  return start == end;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::contains (const Datetime &datetime) const
{
  return (! is_started () || start < datetime) &&
         (! is_ended () || datetime < end);
}

////////////////////////////////////////////////////////////////////////////////
// Detect the following overlap cases:
//
// this                     [--------)
//   A        [--------)
//   B                 [--------)
//   C                        [----)
//   D                           [--------)
//   E                                    [--------)
//   F                    [-------------)
//   G                    [...
//   H                         [...
//   I                                 [...
//
// this                     [...
//   A        [--------)
//   B                 [--------)
//   C                        [----)
//   D                           [--------)
//   E                                    [--------)
//   F                    [-------------)
//   G                    [...
//   H                         [...
//   I                                 [...
//
bool Range::overlaps (const Range &other) const
{
  if (! is_started () || ! other.is_started ())
    return false;

  // Other range ends before this range starts.
  if (other.is_ended () && other.end <= start)
    return false;

  // Other range starts after this range ends.
  if (is_ended () && other.start >= end)
    return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool Range::encloses (const Range& other) const
{
  return other.startsWithin (*this) && other.endsWithin (*this);
}

////////////////////////////////////////////////////////////////////////////////
bool Range::startsWithin (const Range& other) const
{
  if (other.is_empty ())
  {
    return false;
  }

  return other.start == start || other.contains (start);
}

////////////////////////////////////////////////////////////////////////////////
bool Range::endsWithin (const Range& other) const
{
  if (other.is_empty ())
  {
    return false;
  }

  return other.end == end || other.contains (end);
}

////////////////////////////////////////////////////////////////////////////////
// Calculate the following intersection cases:
//
// this                     [--------)
//   A        [--------)
//   B                 [--------)
//   C                        [----)
//   D                           [--------)
//   E                                    [--------)
//   F                    [-------------)
//   G                    [...
//   H                         [...
//   I                                 [...
//
// this                     [...
//   A        [--------)
//   B                 [--------)
//   C                        [----)
//   D                           [--------)
//   E                                    [--------)
//   F                    [-------------)
//   G                    [...
//   H                         [...
//   I                                 [...
//
Range Range::intersect (const Range& other) const
{
  if (overlaps (other))
  {
    // Intersection is choosing the later of the two starts, and the earlier of
    // the two ends, provided the two ranges overlap.
    Range result;
    result.start = start > other.start ? start : other.start;

    if (is_ended ())
    {
      if (other.is_ended ())
        result.end = end < other.end ? end : other.end;
      else
        result.end = end;
    }
    else
    {
      if (other.is_ended ())
        result.end = other.end;
    }

    return result;
  }

  // If there is an intersection but no overlap, we have a zero-width
  // interval [p, p) and another interval [p, q), where q >= p.
  if (intersects (other)) {
    return Range {start, start};
  }

  return Range {};
}

////////////////////////////////////////////////////////////////////////////////
bool Range::intersects (const Range &other) const
{
  if (overlaps (other)) {
    return true;
  }

  // A half-closed zero-width interval [p, p) may have the same
  // starting point as another interval without overlapping it.
  // We consider p to be an element of a range [p, p).
  return (is_started () && other.is_started () && start == other.start);
}

////////////////////////////////////////////////////////////////////////////////
// If the ranges do not overlap, the result is *this.
//
// this   [----)
// other     [----)
// result [-------)
//
// this   [...
// other     [----)
// result [...
//
// this   [----)
// other     [...
// result [...
//
// this   [...
// other     [...
// result [...
//
Range Range::combine (const Range& other) const
{
  Range result {*this};

  if (is_started () && other.is_started ())
  {
    // Start is hte earlier of the two.
    result.start = std::min (result.start, other.start);

    if (is_open () || other.is_open ())
      result.end = {0};
    else
      result.end = std::max (result.end, other.end);
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////
// Consider the following overlap cases:
//
// this                     [--------)
//   A        [--------)
//   B                 [--------)
//   C                        [----)
//   D                           [--------)
//   E                                    [--------)
//   F                    [-------------)
//   G                    [...
//   H                         [...
//   I                                 [...
//
// this                     [...
//   A        [--------)
//   B                 [--------)
//   C                        [----)
//   D                           [--------)
//   E                                    [--------)
//   F                    [-------------)
//   G                    [...
//   H                         [...
//   I                                 [...
//
std::vector <Range> Range::subtract (const Range& other) const
{
  std::vector <Range> results;

  if (overlaps (other))
  {
    if (start < other.start)
    {
      results.push_back (Range (start, other.start));

      if (other.is_ended () &&
          (! is_ended () || end > other.end))
      {
        results.push_back (Range (other.end, end));
      }
    }
    else
    {
      if (other.is_ended ())
      {
        if (is_ended ())
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
// Returns the number of seconds between start and end.
// If the range is open, use 'now' as the end.
time_t Range::total () const
{
  assert (is_open () || end >= start);

  if (is_open ())
    return Datetime () - Datetime (start);

  return Datetime (end) - Datetime (start);
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
