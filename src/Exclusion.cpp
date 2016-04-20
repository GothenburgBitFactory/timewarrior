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
#include <Exclusion.h>
#include <Datetime.h>
#include <Pig.h>
#include <shared.h>
#include <format.h>

////////////////////////////////////////////////////////////////////////////////
// An exclusion represents untrackable time such as holidays, weekends, evenings
// and lunch. By default there are none, but they may be configured. Once there
// are exclusions defined, the :fill functionality is enabled.
//
// Exclusions are instantiated from the 'define exclusions:' rule. This method
// simply validates.
//
// Syntax:
//   exc monday <block> [<block> ...]
//   exc day on <date>
//   exc day off <date>
//
// block:
//   <HH:MM:SS | HH:MM:SS-HH:MM:SS | >HH:MM:SS
//
void Exclusion::initialize (const std::string& line)
{
  _tokens = split (line);

  // Validate syntax only. Do nothing with the data.
  if (_tokens.size () >= 2 &&
      _tokens[0] == "exc")
  {
    if (_tokens.size () == 4 &&
        _tokens[1] == "day"  &&
        _tokens[2] == "on")
    {
      _additive = true;
      return;
    }
    if (_tokens.size () == 4 &&
        _tokens[1] == "day"  &&
        _tokens[2] == "off")
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

  throw format ("Unrecognized exclusion syntax: '{1}'.", line);
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
std::vector <Daterange> Exclusion::ranges (const Daterange& range) const
{
  std::vector <Daterange> results;
  int dayOfWeek;

  if (_tokens[1] == "day" &&
      (_tokens[2] == "on" ||
       _tokens[2] == "off"))
  {
    Datetime start (_tokens[3]);
    Datetime end (start);
    ++end;
    Daterange day (start, end);
    if (range.overlap (day))
      results.push_back (day);
  }

  else if ((dayOfWeek = Datetime::dayOfWeek (_tokens[1])) != -1)
  {
    Datetime start = range.start ();
    while (start < range.end ())
    {
      if (start.dayOfWeek () == dayOfWeek)
      {
        Datetime end = start;
        ++end;

        // Now that 'start' and 'end' respresent the correct day, compose a set
        // of Daterange objects for each time block.
        for (unsigned int b = 2; b < _tokens.size (); ++b)
          results.push_back (daterangeFromTimeBlock (_tokens[b], start, end));
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
std::string Exclusion::serialize () const
{
  return std::string ("exc") + join (" ", _tokens);
}

////////////////////////////////////////////////////////////////////////////////
std::string Exclusion::dump () const
{
  return std::string ("Exclusion ") + join (" ", _tokens) + "\n";
}

////////////////////////////////////////////////////////////////////////////////
Daterange Exclusion::daterangeFromTimeBlock (
  const std::string& block,
  const Datetime& start,
  const Datetime& end) const
{
  Pig pig (block);

  if (pig.skip ('<'))
  {
    int hh, mm, ss;
    if (pig.getHMS (hh, mm, ss))
      return Daterange (start, Datetime (start.month (), start.day (), start.year (), hh, mm, ss));

    throw format ("Malformed time block '{1}'.", block);
  }
  else if (pig.skip ('>'))
  {
    int hh, mm, ss;
    if (pig.getHMS (hh, mm, ss))
      return Daterange (Datetime (start.month (), start.day (), start.year (), hh, mm, ss), end);

    throw format ("Malformed time block '{1}'.", block);
  }

  int hh1, mm1, ss1;
  int hh2, mm2, ss2;
  if (pig.getHMS (hh1, mm1, ss1) &&
      pig.skip ('-')             &&
      pig.getHMS (hh2, mm2, ss2))
    return Daterange (
             Datetime (start.month (), start.day (), start.year (), hh1, mm1, ss1),
             Datetime (start.month (), start.day (), start.year (), hh2, mm2, ss2));

  throw format ("Malformed time block '{1}'.", block);
}

////////////////////////////////////////////////////////////////////////////////
