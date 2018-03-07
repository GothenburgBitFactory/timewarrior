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
// https://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <Exclusion.h>
#include <Datetime.h>
#include <Pig.h>
#include <shared.h>
#include <format.h>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
// An exclusion represents untrackable time such as holidays, weekends, evenings
// and lunch. By default there are none, but they may be configured. Once there
// are exclusions defined, the :fill functionality is enabled.
//
// Exclusions are instantiated from configuration, and are passed here as name/
// value strings.
//
//   Name                        Value
//   --------------------------  ------------------------
//   exclusions.days.2016_01_01  on
//   exclusions.days.2016_01_02  off
//   exclusions.friday           <8:00 12:00-12:45 >17:30
//   exclusions.monday           <8:00 12:00-12:45 >17:30
//   exclusions.thursday         <8:00 12:00-12:45 >17:30
//   exclusions.tuesday          <8:00 12:00-12:45 >18:30
//   exclusions.wednesday        <8:00 12:00-13:30 >17:30
//
Exclusion::Exclusion (const std::string& name, const std::string& value)
//void Exclusion::initialize (const std::string& line)
{
  _tokens = split (name, '.');
  for (auto& token : split (value))
    _tokens.push_back (token);

  // Validate syntax only. Do nothing with the data.
  if (_tokens.size () >= 2 &&
      _tokens[0] == "exclusions")
  {
    if (_tokens.size () == 4 &&
        _tokens[1] == "days"  &&
        _tokens[3] == "on") {
      _additive = true;
      return;
    }
    if (_tokens.size () == 4 &&
        _tokens[1] == "days"  &&
        _tokens[3] == "off")
    {
      _additive = false;
      return;
    }
    else if (Datetime::dayOfWeek (_tokens[1]) != -1)
    {
      _additive = false;
      return;
    }
  }

  throw format ("Unrecognized exclusion syntax: '{1}' '{2}'.", name, value);
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Exclusion::tokens () const
{
  return _tokens;
}

////////////////////////////////////////////////////////////////////////////////
// Within range, yield a collection of recurring ranges as defined by _tokens.
//
//   exc monday <block> [<block> ...]  --> yields a day range for every monday,
//                                         and every block within
//   exc day on <date>                 --> yields single day range
//   exc day off <date>                --> yields single day range
//
std::vector <Range> Exclusion::ranges (const Range& range) const
{
  std::vector <Range> results;
  int dayOfWeek;

  if (_tokens[1] == "days" &&
      (_tokens[3] == "on" ||
       _tokens[3] == "off"))
  {
    auto day = _tokens[2];
    std::replace (day.begin (), day.end (), '_', '-');
    Datetime start (day);
    Datetime end (start);
    ++end;
    Range all_day (start, end);
    if (range.overlap (all_day))
      results.push_back (all_day);
  }

  else if ((dayOfWeek = Datetime::dayOfWeek (_tokens[1])) != -1)
  {
    Datetime start (range.start.year (), range.start.month (), range.start.day (), 0, 0, 0);

    Range myRange = {range};

    if (myRange.is_open())
    {
      myRange.end = Datetime("tomorrow");
    }

    while (start <= myRange.end)
    {
      if (start.dayOfWeek () == dayOfWeek)
      {
        Datetime end = start;
        ++end;

        // Now that 'start' and 'end' represent the correct day, compose a set
        // of Range objects for each time block.
        for (unsigned int block = 2; block < _tokens.size (); ++block)
        {
          auto r = rangeFromTimeBlock (_tokens[block], start, end);
          if (myRange.overlap (r))
            results.push_back (r);
        }
      }

      ++start;
    }
  }

  return results;
}

////////////////////////////////////////////////////////////////////////////////
bool Exclusion::additive () const
{
  return _additive;
}

////////////////////////////////////////////////////////////////////////////////
std::string Exclusion::dump () const
{
  return join (" ", _tokens);
}

////////////////////////////////////////////////////////////////////////////////
Range Exclusion::rangeFromTimeBlock (
  const std::string& block,
  const Datetime& start,
  const Datetime& end) const
{
  Pig pig (block);

  if (pig.skip ('<'))
  {
    int hh, mm, ss;
    if (pig.getHMS (hh, mm, ss))
      return Range (Datetime (start.year (), start.month (), start.day (),  0,  0,  0),
                    Datetime (start.year (), start.month (), start.day (), hh, mm, ss));
  }
  else if (pig.skip ('>'))
  {
    int hh, mm, ss;
    if (pig.getHMS (hh, mm, ss))
      return Range (Datetime (start.year (), start.month (), start.day (), hh, mm, ss),
                    Datetime (end.year (),   end.month (),   end.day (),    0,  0,  0));
  }
  else
  {
    int hh1, mm1, ss1;
    int hh2, mm2, ss2;
    if (pig.getHMS (hh1, mm1, ss1) &&
        pig.skip ('-')             &&
        pig.getHMS (hh2, mm2, ss2))
      return Range (
               Datetime (start.year (), start.month (), start.day (), hh1, mm1, ss1),
               Datetime (start.year (), start.month (), start.day (), hh2, mm2, ss2));
  }

  throw format ("Malformed time block '{1}'.", block);
}

////////////////////////////////////////////////////////////////////////////////
