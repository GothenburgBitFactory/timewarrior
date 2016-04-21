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
#include <timew.h>
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

  // Create a range representing the whole timeline.
  // If no range is defined, then assume the full range of all the inclusions.
  Daterange timelineRange {_range};
  if (timelineRange.start ().toEpoch () == 0 &&
      timelineRange.end ().toEpoch () == 0)
  {
    for (auto& inclusion : _inclusions)
    {
      if (inclusion.start () < timelineRange.start () || timelineRange.start ().toEpoch () == 0)
        timelineRange.start (inclusion.start ());

      // Deliberately mixed start/end.
      if (inclusion.start () > timelineRange.end ())
        timelineRange.end (inclusion.start ());

      if (inclusion.end () > timelineRange.end ())
        timelineRange.end (inclusion.end ());
    }

    std::cout << "# Timeline augmented range: " << timelineRange.start ().toISO () << " - " << timelineRange.end ().toISO () << "\n";
  }

  // TODO Return results, which should be the stored inclusions, clipped by
  //      subtracting all the exclusions nad holidays.

/*
  std::vector <Interval> combined;

  return combined;
*/

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
