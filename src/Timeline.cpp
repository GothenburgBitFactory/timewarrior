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
//         [---------------------------------------)          Inclusion
//   [--------)          [--------)          [--------)       Exclusion
//
// Derived:
//
//         [-------------)        [----------------)          Tracked
//   [-----)                                          [-----) Gaps
//
////////////////////////////////////////////////////////////////////////////////

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
  // Get the set of expanded exclusions that overlap the range defined by the
  // timeline. If no range is defined, derive it from the set of all data.
  auto exclusions = excluded (rules);

  std::vector <Interval> all;
  for (auto& interval : _inclusions)
  {
    // Closed intervals are returned as-is.
    if (interval.range.ended ())
    {
      all.push_back (interval);
    }

    // Open intervals need to be split according to the exclusions.
    else
    {
      for (auto& fragment : splitInterval (interval, exclusions))
        all.push_back (fragment);
    }
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
// Untracked time is that which is not excluded, and not filled. Gaps.
std::vector <Range> Timeline::untracked (Rules& rules) const
{
  std::vector <Range> gaps;

  // Get the set of expanded exclusions that overlap the range defined by the
  // timeline. If no range is defined, derive it from the set of all data.
  auto exclusions = excluded (rules);

  // TODO subtract all exclusions
  // TODO subtract all inclusions

  return gaps;
}

////////////////////////////////////////////////////////////////////////////////
// Excluded time is that which is not available for work.
std::vector <Range> Timeline::excluded (Rules& rules) const
{
  // Create a range representing the whole timeline.
  // If no range is defined, then assume the full range of all the inclusions.
  Range overallRange {range};
  if (! overallRange.started () &&
      ! overallRange.ended ())
    overallRange = overallRangeFromIntervals (_inclusions);

  // Cobmine all the non-trackable time.
  return combineHolidaysAndExclusions (overallRange, rules, _exclusions);
}

////////////////////////////////////////////////////////////////////////////////
std::string Timeline::dump () const
{
  std::stringstream out;

  out << "Timeline range " << range.dump () << '\n';
  for (auto& i : _inclusions)
    out << "  " << i.json ();
  for (auto& e : _exclusions)
    out << "  " << e.dump ();

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
