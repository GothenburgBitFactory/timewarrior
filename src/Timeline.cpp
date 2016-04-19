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
#include <Timeline.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
// The Timeline object represents a continuum with a defined start and end
// point. Between these two points there may be incluѕions (tracked time), and
// exclusions (untrackable time). When all known incluѕions and exclusions are
// added to a timeline, it is possible to extract (a) untracked gaps, and (b)
// blocks of tracked time. For example:
//
// Inputs:
//
//   [------------------------------------------------------) Timeline
//           [-------------------------------------)          Inclusion
//   [--------)          [--------)          [--------)       Exclusion
//
// Derived:
//
//           [-----------)        [----------------)          Tracked
//   [-------)                                        [-----) Gaps
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void Timeline::range (const Daterange& range)
{
  _range = range;
}

////////////////////////////////////////////////////////////////////////////////
void Timeline::include (const Interval& interval)
{
  _inclusions.push_back (interval);
}

////////////////////////////////////////////////////////////////////////////////
void Timeline::exclude (const Exclusion& exclusion)
{
  _exclusions.push_back (exclusion);
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> Timeline::tracked (Rules& rules) const
{
  // TODO Create a range representing the whole timeline.

  // TODO Intersect this range with the outer range of all inclusions.

  // TODO Obtain holidays.
  // TODO for each exclusion:
  // TODO   if "exc day off <date>":
  // TODO     add <date> as a holiday.
  // TODO   if "exc day on <date>":
  // TODO     if <date> matches a holiday:
  // TODO       remove <date> as a holiday

  // TODO for each inclusion:
  // TODO   for each holiday:
  // TODO     intersect intelligently.

  // TODO   for each exclusion:
  // TODO     generate a set of ranges to exclude.
  // TODO     for each excluded range:
  // TODO       intersect intelligently.

  // TODO Return results, which should be the stored inclusions, clipped by
  //      subtracting all the exclusions nad holidays.

/*
  std::vector <Interval> combined;

  return combined;
*/

  return _inclusions;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> Timeline::untracked (Rules& rules) const
{
  std::vector <Interval> combined;

  // TODO Combine _inclusions and _exclusions to yield a set of collapsed
  //      unracked intervals.

  return combined;
}

////////////////////////////////////////////////////////////////////////////////
std::string Timeline::dump () const
{
  std::stringstream out;

  out << "Timeline _range " << _range.start ().toISO () << " - " << _range.end ().toISO () << "\n";
  for (auto& i : _inclusions)
    out << "  " << i.dump ();
  for (auto& e : _exclusions)
    out << "  " << e.dump ();

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
