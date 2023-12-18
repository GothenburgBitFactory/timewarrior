////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <IntervalFilterAllInRange.h>

IntervalFilterAllInRange::IntervalFilterAllInRange (Range range): _range (std::move(range))
{}

bool IntervalFilterAllInRange::accepts (const Interval& interval)
{
  if (is_done())
  {
    return false;
  }

  if ((! _range.is_started() && ! _range.is_ended()) || interval.intersects (_range))
  {
    return true;
  }

  // Since we are moving backwards in time, and the intervals are in sorted order,
  // if the filter is after the interval and does not intersect,
  // we know there will be no more matches
  set_done (interval.start < _range.start);

  return false;
}
