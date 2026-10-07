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
// https://opensource.org/license/mit
//
////////////////////////////////////////////////////////////////////////////////

#include <Datetime.h>
#include <Duration.h>
#include <IntervalFactory.h>
#include <IntervalFilter.h>
#include <JSON.h>
#include <algorithm>
#include <format.h>
#include <map>
#include <memory>
#include <shared.h>
#include <sstream>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
// Read rules and extract all holiday definitions. Create a Range for each
// one that spans from midnight to midnight.
std::vector <Range> getHolidays (const Rules& rules)
{
  std::vector <Range> results;
  for (auto& holiday : rules.all ("holidays."))
  {
    // Skip the provider setting itself – it is not a date entry.
    if (holiday == "holidays.provider")
      continue;

    auto lastDot = holiday.rfind ('.');
    if (lastDot != std::string::npos)
    {
      try
      {
        Range r;
        Datetime d (holiday.substr (lastDot + 1), "Y_M_D");
        r.start = d;
        ++d;
        r.end = d;
        results.push_back (r);
      }
      catch (...)
      {
        // Skip entries whose trailing segment is not a valid Y_M_D date
      }
    }
  }

  debug (format ("Found {1} holidays", results.size ()));
  return results;
}

////////////////////////////////////////////////////////////////////////////////
// Call the configured external holiday provider tool with a date range and
// parse its JSON Lines output into (date, name) pairs.
//
// Each output line must be a JSON object with "date" (YYYY-MM-DD) and "name":
//   {"date":"2025-01-01","name":"New Year's Day"}
//
static std::vector <std::pair <Datetime, std::string>> getHolidayEntriesFromProvider (
  const std::string& provider,
  const Range& range)
{
  auto start_str = range.start.toString ("Y-M-D");
  auto end_str   = range.is_ended () ? range.end.toString ("Y-M-D")
                                     : Datetime ().toString ("Y-M-D");

  std::string output;
  if (execute (provider, {start_str, end_str}, "", output) != 0)
  {
    warn (format ("holidays.provider '{1}' exited with error", provider));
    return {};
  }

  std::vector <std::pair <Datetime, std::string>> results;
  for (auto& line : split (output, '\n'))
  {
    if (line.empty ())
      continue;

    try
    {
      json::value* raw = json::parse (line);
      if (! raw)
        continue;

      if (raw->type () != json::j_object)
      {
        delete raw;
        continue;
      }

      std::unique_ptr <json::object> obj (static_cast <json::object*> (raw));

      auto date_it = obj->_data.find ("date");
      auto name_it = obj->_data.find ("name");
      if (date_it == obj->_data.end () || name_it == obj->_data.end ())
        continue;

      auto* date_val = dynamic_cast <json::string*> (date_it->second);
      auto* name_val = dynamic_cast <json::string*> (name_it->second);
      if (! date_val || ! name_val)
        continue;

      Datetime d (json::decode (date_val->_data), "Y-M-D");
      results.emplace_back (d, json::decode (name_val->_data));
    }
    catch (...)
    {
      // Skip malformed JSON lines
    }
  }

  debug (format ("Found {1} holidays from provider", results.size ()));
  return results;
}

////////////////////////////////////////////////////////////////////////////////
// Range-aware variant: if holidays.provider is configured, call the external
// tool with the given date range; otherwise fall back to the config-file data.
std::vector <Range> getHolidays (const Rules& rules, const Range& range)
{
  auto provider = json::decode (rules.get ("holidays.provider"));
  if (! provider.empty ())
  {
    std::vector <Range> results;
    for (auto& entry : getHolidayEntriesFromProvider (provider, range))
    {
      Datetime d = entry.first;
      Range r;
      r.start = d;
      ++d;
      r.end = d;
      results.push_back (r);
    }
    return results;
  }

  return getHolidays (rules);
}

////////////////////////////////////////////////////////////////////////////////
// Read rules and extract all holiday definitions within range. Create a map
// from date to holiday description string (with locale).
//
// If holidays.provider is configured, call the external tool instead of
// reading from the config file.  Provider output uses only the holiday name
// (no locale bracket); config-file entries retain their " [locale] name" form.
std::map <Datetime, std::string> createHolidayMap (Rules& rules, Range& range)
{
  std::map <Datetime, std::string> mapping;

  auto provider = json::decode (rules.get ("holidays.provider"));
  if (! provider.empty ())
  {
    for (auto& entry : getHolidayEntriesFromProvider (provider, range))
      mapping[entry.first] = " " + entry.second;
    return mapping;
  }

  auto holidays = rules.all ("holidays.");

  for (auto& entry : holidays)
  {
    // Skip the provider setting itself – it is not a date entry.
    if (entry == "holidays.provider")
      continue;

    auto first_dot = entry.find ('.');
    auto last_dot = entry.rfind ('.');

    if (last_dot != std::string::npos)
    {
      auto date = entry.substr (last_dot + 1);
      std::replace (date.begin (), date.end (), '_', '-');
      Datetime holiday (date);

      if (holiday >= range.start && holiday <= range.end)
      {
        std::stringstream out;
        out << " ["
            << entry.substr (first_dot + 1, last_dot - first_dot - 1)
            << "] "
            << rules.get (entry);
        mapping[holiday] = out.str ();
      }
    }
  }

  return mapping;
}

////////////////////////////////////////////////////////////////////////////////
// Render a map of holidays as a formatted string.
std::string renderHolidays (const std::map <Datetime, std::string>& holidays)
{
  std::stringstream out;

  for (auto& entry : holidays)
  {
    out << entry.first.toString ("Y-M-D")
        << " "
        << entry.second
        << '\n';
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// [1] Read holiday definitions from the rules, extract their dates and create
//     a set of Range from them.
// [2] For 'exc day ...' exclusions, separate into daysOn and daysOff sets,
//     based on whether the exclusion is additive.
// [3] Treat daysOff as additional holidays.
// [4] Subtract daysOn from the set of holidays.
// [5] Take all the 'exc <dayOfWeek> ...' exclusions and expand them into
//     concrete ranges within the overall range, adding them to the results.
//
// The result is the complete set of untrackable time that lies within the
// input range. This will be a set of nights, weekends, holidays and lunchtimes.
std::vector <Range> getAllExclusions (
  const Rules& rules,
  const Range& range)
{
  // Start with the set of all holidays, intersected with range.
  std::vector <Range> results;
  results = addRanges (range, results, getHolidays (rules, range));

  // Load all exclusions from configuration.
  std::vector <Exclusion> exclusions;
  for (auto& name : rules.all ("exclusions."))
    exclusions.emplace_back (lowerCase (name), rules.get (name));
  debug (format ("Found {1} exclusions", exclusions.size ()));

  // Find exclusions 'exc day on <date>' and remove from holidays.
  // Find exclusions 'exc day off <date>' and add to holidays.
  std::vector <Range> daysOn;
  std::vector <Range> daysOff;
  for (auto& exclusion : exclusions)
  {
    if (exclusion.tokens ()[1] == "day")
    {
      if (exclusion.additive ())
        for (auto& r : exclusion.ranges (range))
          daysOn.push_back (r);
      else
        for (auto& r : exclusion.ranges (range))
          daysOff.push_back (r);
    }
  }

  // daysOff are combined with existing holidays.
  results = addRanges (range, results, daysOff);
  if (! daysOn.empty ())
    debug (format ("Found {1} additional working days", daysOn.size ()));
  if (! daysOff.empty ())
    debug (format ("Found {1} additional non-working days", daysOff.size ()));

  // daysOn are subtracted from the existing holidays.
  results = subtractRanges (results, daysOn);

  // Expand all exclusions that are not 'exc day ...' into excluded ranges that
  // overlap with range.
  std::vector <Range> exclusionRanges;
  for (auto& exclusion : exclusions)
  {
    if (exclusion.tokens ()[1] != "day")
    {
      for (auto& r : exclusion.ranges (range))
      {
        if (! r.is_empty ())
        {
          exclusionRanges.push_back (r);
        }
      }
    }
  }

  return merge (addRanges (range, results, exclusionRanges));
}

////////////////////////////////////////////////////////////////////////////////
// Potentially expand the latest interval into a collection of synthetic
// intervals.
std::vector <Interval> expandLatest (const Interval& latest, const Rules& rules)
{
  int current_id = 0;
  std::vector <Interval> intervals;
  // If the latest interval is open, check for synthetic intervals
  if (latest.is_open ())
  {
    auto exclusions = getAllExclusions (rules, {latest.start, Datetime ()});
    if (! exclusions.empty ())
    {
      std::vector <Interval> flattened = flatten (latest, exclusions);

      // If flatten() converted the latest interval into a group of synthetic
      // intervals, the number of returned intervals will be greater than 1.
      // Otherwise, it just returned the non-synthetic, latest interval.
      if (flattened.size () > 1)
      {
        std::reverse (flattened.begin (), flattened.end ());
        for (auto interval : flattened)
        {
          interval.synthetic = true;
          interval.id = ++current_id;
          intervals.push_back (std::move (interval));
        }
      }
    }
  }

  if (intervals.empty ())
  {
    intervals.push_back (latest);
    intervals.back ().id = 1;
  }
  return intervals;
}

////////////////////////////////////////////////////////////////////////////////
// Convert what would be synthetic intervals into real intervals in the database
void flattenDatabase (Database& database, const Rules& rules)
{
  Interval latest = getLatestInterval (database);
  std::vector <Interval> expanded = expandLatest (latest, rules);

  if (expanded.size () > 1)
  {
    bool verbose = rules.getBoolean ("verbose");
    database.deleteInterval (latest);
    for (auto it = expanded.rbegin (); it != expanded.rend (); ++it)
    {
      it->synthetic = false;
      database.addInterval (*it, verbose);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Range> subset (
  const Range& range,
  const std::vector <Range>& ranges)
{
  std::vector <Range> all;
  for (auto& r : ranges) {
    if (range.intersects (r)) {
      all.push_back (r);
    }
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> subset (
  const Range& range,
  const std::vector <Interval>& intervals)
{
  std::vector <Interval> all;
  for (auto& interval : intervals) {
    if (range.intersects (interval))
    {
      all.push_back (interval);
    }
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <Interval> flatten (
  const Interval& interval,
  const std::vector <Range>& exclusions)
{
  std::vector <Interval> all;

  std::vector <Range> enclosed;
  for (auto& e : exclusions)
    if (interval.encloses (e))
      enclosed.push_back (e);

  Datetime now;
  for (auto& result : subtractRanges ({interval}, enclosed))
  {
    if (interval.is_open () && result.start > now)
    {
      break;
    }

    Interval chunk {interval};
    chunk.setRange (result);

    all.push_back (chunk);
  }

  return all;
}

////////////////////////////////////////////////////////////////////////////////
// Simply merges a vector of ranges, without data loss.
static bool rangeCompare (Range left, Range right)
{
  return left.start < right.start;
}

std::vector <Range> merge (
  const std::vector <Range>& ranges)
{
  // Shortcut.
  if (ranges.size () < 2)
    return ranges;

  std::vector <Range> sorted {ranges};
  std::sort (sorted.begin (), sorted.end (), rangeCompare);

  unsigned int cursor = 0;
  int merges = 0;
  for (unsigned int i = 0; i < sorted.size (); ++i)
  {
    if (cursor && sorted[cursor - 1].overlaps (sorted[i]))
    {
      sorted[cursor - 1] = sorted[cursor - 1].combine (sorted[i]);
      ++merges;
    }
    else
    {
      sorted[cursor] = sorted[i];
      ++cursor;
    }
  }

  if (merges)
    sorted.resize (ranges.size () - merges);

  return sorted;
}

////////////////////////////////////////////////////////////////////////////////
// Subset both ranges and additions by limits, and combine.
std::vector <Range> addRanges (
  const Range& limits,
  const std::vector <Range>& ranges,
  const std::vector <Range>& additions)
{
  std::vector <Range> results;

  for (auto& range : ranges)
    if (limits.overlaps (range))
      results.push_back (range);

  for (auto& addition : additions)
    if (limits.overlaps (addition))
      results.push_back (addition);

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// Subtract a set of Ranges from another set of Ranges, all within a defined range.
std::vector <Range> subtractRanges (
  const std::vector <Range>& ranges,
  const std::vector <Range>& subtractions)
{
  std::vector <Range> results = ranges;
  for (auto& s : subtractions)
  {
    std::vector <Range> split_results;
    for (auto& range : results)
      for (auto& split_range : range.subtract (s))
        split_results.push_back (split_range);

    results = split_results;
  }

  return results;
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter range if the start/end overlaps
bool matchesRange (const Interval& interval, const Range& filter)
{
  return ((! filter.is_started () && ! filter.is_ended ()) || interval.intersects (filter));
}

////////////////////////////////////////////////////////////////////////////////
// An interval matches a filter interval if the start/end overlaps, and all
// filter interval tags are found in the interval.
//
// [1] interval.end.toEpoch () == 0
// [2] interval.end > filter.start
// [3] filter.end.toEpoch () == 0
// [4] interval.start < filter.end
//
// Match:   (1 || 2) && (3 || 4)

// filter closed         [--------)                  1  2  3  4  5  6  result
// --------------------------------------------------------------------------
// A       [--------)    .        .                  0  0  0  1  0  0       0
// B                [--------)    .                  0  1  0  1  1  1       1
// C                     . [----) .                  0  1  0  1  1  1       1
// D                     .    [--------)             0  1  0  1  1  1       1
// E                     .        .    [--------)    0  1  0  0  1  0       0
// F                   [-------------)               0  1  0  1  1  1       1
// G                   [...       .                  1  0  0  1  1  1       1
// H                     .  [...  .                  1  0  0  1  1  1       1
// I                     .        . [...             1  0  0  0  1  0       0
//
//
//
// filter open           [...                        1  2  3  4  5  6  result
// --------------------------------------------------------------------------
// A       [--------)    .                           0  0  1  0  0  1       0
// B                [--------)                       0  1  1  0  1  1       1
// C                     . [----)                    0  1  1  0  1  1       1
// D                     .    [--------)             0  1  1  0  1  1       1
// E                     .             [--------)    0  1  1  0  1  1       1
// F                   [-------------)               0  1  1  0  1  1       1
// G                   [...                          1  0  1  0  1  1       1
// H                     .  [...                     1  0  1  0  1  1       1
// I                     .          [...             1  0  1  0  1  1       1
//
bool matchesFilter (const Interval& interval, const Interval& filter)
{
  if (matchesRange (interval, filter))
  {
    for (auto& tag : filter.tags ())
    {
      if (! interval.hasTag (tag))
      {
        return false;
      }
    }
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Take an interval and clip it to the range
Interval clip (const Interval& interval, const Range& range)
{
  if (! range.is_started () ||
      range.total () == 0)
    return interval;

  Interval clipped {interval};
  if (clipped.start.toEpoch () &&
      clipped.start < range.start)
    clipped.start = range.start;

  if (clipped.end.toEpoch () &&
      clipped.end > range.end)
    clipped.end = range.end;

  return clipped;
}

////////////////////////////////////////////////////////////////////////////////
// Return collection of intervals that match the filter (synthetic intervals
// included) sorted by date
std::vector <Interval> getTracked (
  Database& database,
  const Rules& rules,
  IntervalFilter& filter)
{
  int current_id = 0;
  std::vector <Interval> intervals;

  auto it = database.begin ();
  auto end = database.end ();

  // Because the latest recorded interval may be expanded into synthetic
  // intervals, we'll handle it specially
  if (it != end )
  {
    Interval latest = IntervalFactory::fromSerialization (*it);
    ++it;

    for (auto& interval : expandLatest (latest, rules))
    {
      ++current_id;
      if (filter.accepts (interval))
      {
        interval.id = current_id;
        intervals.push_back (interval);
      }
      else if (filter.is_done ())
      {
        break;
      }
    }
  }

  for (; it != end; ++it)
  {
    Interval interval = IntervalFactory::fromSerialization (*it);
    interval.id = ++current_id;

    if (filter.accepts (interval))
    {
      intervals.push_back (std::move (interval));
    }
    else if (filter.is_done ())
    {
      // Since we are moving backwards in time, and the intervals are in sorted
      // order, if the filter is after the interval, we know there will be no
      // more matches
      break;
    }
  }

  debug (format ("Loaded {1} tracked intervals", intervals.size ()));

  // By default, intervals are sorted by id, but getTracked needs to return the
  // intervals sorted by date, which are ids in reverse order.
  std::reverse (intervals.begin (), intervals.end ());

  return intervals;
}

////////////////////////////////////////////////////////////////////////////////
// Untracked time is that which is not excluded, and not filled. Gaps.
std::vector <Range> getUntracked (
  Database& database,
  const Rules& rules,
  Interval& filter)
{
  bool found_match = false;
  std::vector <Range> inclusion_ranges;
  for (auto& line : database)
  {
    Interval i = IntervalFactory::fromSerialization (line);
    if (matchesFilter (i, filter))
    {
      inclusion_ranges.push_back (i);
      found_match = true;
    }
    else if (found_match)
    {
      // If we already had a match, and now we do not, since the database is in
      // order from most recent to the oldest inclusion, we can be sure that there
      // will not be any further matches.
      break;
    }
  }

  auto available = subtractRanges ({filter}, getAllExclusions (rules, filter));
  auto untracked = subtractRanges (available, inclusion_ranges);
  debug (format ("Loaded {1} untracked ranges", untracked.size ()));
  return untracked;
}

////////////////////////////////////////////////////////////////////////////////
Interval getLatestInterval (Database& database)
{
  Interval i;
  auto latestEntry = database.getLatestEntry ();
  if (! latestEntry.empty ())
  {
    i = IntervalFactory::fromSerialization (latestEntry);
    i.id = 1;
  }

  return i;
}

////////////////////////////////////////////////////////////////////////////////
Range getFullDay (const Datetime& day)
{
  int y;
  int m;
  int d;
  day.toYMD (y, m, d);
  return Range (Datetime (y, m, d, 0, 0, 0),
                Datetime (y, m, d, 24, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////
