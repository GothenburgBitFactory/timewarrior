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
#include <Daterange.h>

////////////////////////////////////////////////////////////////////////////////
Daterange::Daterange (const Datetime& start, const Datetime& end)
{
  _start = start;
  _end = end;
}

////////////////////////////////////////////////////////////////////////////////
bool Daterange::operator== (const Daterange& other)
{
  return _start == other._start &&
         _end   == other._end;
}

////////////////////////////////////////////////////////////////////////////////
Datetime Daterange::start () const
{
  return _start;
}

////////////////////////////////////////////////////////////////////////////////
void Daterange::start (const Datetime& value)
{
  _start = value;
}

////////////////////////////////////////////////////////////////////////////////
Datetime Daterange::end () const
{
  return _end;
}

////////////////////////////////////////////////////////////////////////////////
void Daterange::end (const Datetime& value)
{
  _end = value;
}

////////////////////////////////////////////////////////////////////////////////
bool Daterange::isStarted () const
{
  return _start.toEpoch () > 0;
}

////////////////////////////////////////////////////////////////////////////////
bool Daterange::isEnded () const
{
  return _end.toEpoch   () > 0;
}

////////////////////////////////////////////////////////////////////////////////
// Detect the following overlap cases:
//
// this                     |--------|
// A          |--------|
// B                   |--------|
// C                          |----|
// D                             |--------|
// E                                      |--------|
// F                      |-------------|
// G                      |...
// H                           |...
// I                                   |...
//
// this                     |...
// A          |--------|
// B                   |--------|
// C                          |----|
// D                             |--------|
// E                                      |--------|
// F                      |-------------|
// G                      |...
// H                           |...
// I                                   |...
//
bool Daterange::overlap (const Daterange& other) const
{
  if (! isStarted () ||
      ! other.isStarted ())
    return false;

  // Other range ends before this range starts.
  if (other.isEnded () &&
      other.end () <= start ())
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
// B                   |--------|
// C                          |----|
// D                             |--------|
// F                      |-------------|
// G                      |...
// H                           |...
//
// this                     |...
// A          |--------|
// B                   |--------|
// C                          |----|
// D                             |--------|
// E                                      |--------|
// F                      |-------------|
// G                      |...
// H                           |...
// I                                   |...
//
Daterange Daterange::intersect (const Daterange& other) const
{
  Daterange result;

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
// B                   |--------|
// C                          |----|
// D                             |--------|
// F                      |-------------|
// G                      |...
// H                           |...
//
// this                     |...
// A          |--------|
// B                   |--------|
// C                          |----|
// D                             |--------|
// E                                      |--------|
// F                      |-------------|
// G                      |...
// H                           |...
// I                                   |...
//
std::vector <Daterange> Daterange::subtract (const Daterange& other) const
{
  std::vector <Daterange> results;

  if (overlap (other))
  {
    // B C D F G H
    if (start () < other.start ())
    {
      // C D H
      results.push_back (Daterange (start (), other.start ()));

      if (other.isEnded () &&
          end () > other.end ())
      {
        // C
        results.push_back (Daterange (other.end (), end ()));
      }
    }
    else
    {
      // B F G
      if (other.isEnded () &&
          end () > other.end ())
      {
        // B
        results.push_back (Daterange (other.end (), end ()));
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
