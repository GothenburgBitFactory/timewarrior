////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez
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
#include <DatetimeParser.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <stdlib.h>
#include <shared.h>
#include <format.h>
#include <unicode.h>
#include <utf8.h>
#include <src/libshared/src/Duration.h>

static std::vector <std::string> dayNames {
  "sunday",
  "monday",
  "tuesday",
  "wednesday",
  "thursday",
  "friday",
  "saturday"};

static std::vector <std::string> monthNames {
  "january",
  "february",
  "march",
  "april",
  "may",
  "june",
  "july",
  "august",
  "september",
  "october",
  "november",
  "december"};

////////////////////////////////////////////////////////////////////////////////
Range DatetimeParser::parse_range (const std::string& input)
{
  clear ();
  std::string::size_type start = 0;
  auto i = start;
  Pig pig (input);
  if (i)
    pig.skipN (static_cast <int> (i));

  auto checkpoint = pig.cursor ();

  // Parse epoch first, as it's the most common scenario.
  if (parse_epoch (pig))
  {
    // ::validate and ::resolve are not needed in this case.
    start = pig.cursor ();
    return Range {};
  }

  // Allow parse_date_time and parse_date_time_ext regardless of
  // DatetimeParser::isoEnabled setting, because these formats are relied upon by
  // the 'import' command, JSON parser and hook system.
  if (parse_date_time_ext   (pig) || // Strictest first.
      parse_date_time       (pig))
  {
    // Check the values and determine time_t.
    if (validate ())
    {
      start = pig.cursor ();
      resolve ();
      return Range { Datetime {_date}, 0 };
    }
  }

  // Allow parse_date_time and parse_date_time_ext regardless of
  // DatetimeParser::isoEnabled setting, because these formats are relied upon by
  // the 'import' command, JSON parser and hook system.
  if (Datetime::isoEnabled &&
      (                                   parse_date_ext      (pig)  ||
      (Datetime::standaloneDateEnabled && parse_date          (pig))
      )
    )
  {
    // Check the values and determine time_t.
    if (validate ())
    {
      start = pig.cursor ();
      resolve ();

      if (_day != 0)
      {
        auto start_date = Datetime (_date);
        auto end_date = start_date + Duration ("1d").toTime_t ();
        return Range{start_date, end_date };
      }
      else if (_month != 0)
      {
        auto start_date = Datetime (_date);
        auto end_date = Datetime(start_date.year(), start_date.month()+1, 1);
        return Range { start_date, end_date };
      }
      else if (_year != 0)
      {
        auto start_date = Datetime (_date);
        auto end_date = Datetime(start_date.year()+1, 1, 1);
        return Range { start_date, end_date };
      }
      return Range {};
    }
  }

  // Allow parse_date_time and parse_date_time_ext regardless of
  // DatetimeParser::isoEnabled setting, because these formats are relied upon by
  // the 'import' command, JSON parser and hook system.
  if (Datetime::isoEnabled &&
      (                                   parse_time_utc_ext  (pig)  ||
                                          parse_time_utc      (pig)  ||
                                          parse_time_off_ext  (pig)  ||
                                          parse_time_off      (pig)  ||
                                          parse_time_ext      (pig)  ||
      (Datetime::standaloneTimeEnabled && parse_time          (pig)) // Time last, as it is the most permissive.
      )
     )
  {
    // Check the values and determine time_t.
    if (validate ())
    {
      start = pig.cursor ();
      resolve ();
      return Range { Datetime (_date), 0 };
    }
  }

  pig.restoreTo (checkpoint);

  if (parse_informal_time (pig))
  {
    return Range { Datetime {_date}, 0 };
  }

  if (parse_named_day (pig))
  {
    // ::validate and ::resolve are not needed in this case.
    start = pig.cursor ();
    return Range { Datetime (_date), Datetime (_date) + Duration ("1d").toTime_t () };
  }

  if (parse_named_month (pig))
  {
    // ::validate and ::resolve are not needed in this case.
    start = pig.cursor ();
    auto begin = Datetime (_date);
    auto month = (begin.month() + 1) % 13 + (begin.month() == 12);
    auto year = (begin.year() + (begin.month() == 12));
    auto end = Datetime (year, month, 1);
    return Range { begin, end };
  }

  if (parse_named (pig))
  {
    // ::validate and ::resolve are not needed in this case.
    start = pig.cursor ();
    return Range { Datetime (_date), 0 };
  }

  throw format ("'{1}' is not a valid range.", input);
}

////////////////////////////////////////////////////////////////////////////////
void DatetimeParser::clear ()
{
  _year    = 0;
  _month   = 0;
  _week    = 0;
  _weekday = 0;
  _julian  = 0;
  _day     = 0;
  _seconds = 0;
  _offset  = 0;
  _utc     = false;
  _date    = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Note how these are all single words.
//
// Examples and descriptions, assuming now == 2017-03-05T12:34:56.
//
//                  Example              Notes
//                  -------------------  ------------------
//   yesterday      2017-03-04T00:00:00  Unaffected
//   today          2017-03-05T00:00:00  Unaffected
//   tomorrow       2017-03-06T00:00:00  Unaffected
//   <ordinal> 12th 2017-03-12T00:00:00
//   <day> monday   2017-03-06T00:00:00
//   easter         2017-04-16T00:00:00
//   eastermonday   2017-04-16T00:00:00
//   ascension      2017-05-25T00:00:00
//   pentecost      2017-06-04T00:00:00
//   goodfriday     2017-04-14T00:00:00
//   midsommar      2017-06-24T00:00:00  midnight, 1st Saturday after 20th June
//   midsommarafton 2017-06-23T00:00:00  midnight, 1st Friday after 19th June
//   juhannus       2017-06-23T00:00:00  midnight, 1st Friday after 19th June
bool DatetimeParser::parse_named_day (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (initializeYesterday      (pig) ||
      initializeToday          (pig) ||
      initializeTomorrow       (pig) ||
      initializeOrdinal        (pig) ||
      initializeDayName        (pig) ||
      initializeEaster         (pig) ||
      initializeMidsommar      (pig) ||
      initializeMidsommarafton (pig))
  {
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_named_month (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (initializeMonthName (pig))
  {
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_informal_time (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (initializeInformalTime (pig))
  {
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Note how these are all single words.
//
// Examples and descriptions, assuming now == 2017-03-05T12:34:56.
//
//                  Example              Notes
//                  -------------------  ------------------
//   now            2017-03-05T12:34:56  Unaffected
//   yesterday      2017-03-04T00:00:00  Unaffected
//   today          2017-03-05T00:00:00  Unaffected
//   tomorrow       2017-03-06T00:00:00  Unaffected
//   <ordinal> 12th 2017-03-12T00:00:00
//   <day> monday   2017-03-06T00:00:00
//   <month> april  2017-04-01T00:00:00
//   later          2038-01-18T00:00:00  Unaffected
//   someday        2038-01-18T00:00:00  Unaffected
//   sopd           2017-03-04T00:00:00  Unaffected
//   sod            2017-03-05T00:00:00  Unaffected
//   sond           2017-03-06T00:00:00  Unaffected
//   eopd           2017-03-05T00:00:00  Unaffected
//   eod            2017-03-06T00:00:00  Unaffected
//   eond           2017-03-07T00:00:00  Unaffected
//   sopw           2017-02-26T00:00:00  Unaffected
//   sow            2017-03-05T00:00:00  Unaffected
//   sonw           2017-03-12T00:00:00  Unaffected
//   eopw           2017-03-05T00:00:00  Unaffected
//   eow            2017-03-12T00:00:00  Unaffected
//   eonw           2017-03-19T00:00:00  Unaffected
//   sopww          2017-02-27T00:00:00  Unaffected
//   soww           2017-03-06T00:00:00
//   sonww          2017-03-06T00:00:00  Unaffected
//   eopww          2017-03-03T00:00:00  Unaffected
//   eoww           2017-03-10T00:00:00
//   eonww          2017-03-17T00:00:00  Unaffected
//   sopm           2017-02-01T00:00:00  Unaffected
//   som            2017-03-01T00:00:00  Unaffected
//   sonm           2017-04-01T00:00:00  Unaffected
//   eopm           2017-03-01T00:00:00  Unaffected
//   eom            2017-04-01T00:00:00  Unaffected
//   eonm           2017-05-01T00:00:00  Unaffected
//   sopq           2017-10-01T00:00:00  Unaffected
//   soq            2017-01-01T00:00:00  Unaffected
//   sonq           2017-04-01T00:00:00  Unaffected
//   eopq           2017-01-01T00:00:00  Unaffected
//   eoq            2017-04-01T00:00:00  Unaffected
//   eonq           2017-07-01T00:00:00  Unaffected
//   sopy           2016-01-01T00:00:00  Unaffected
//   soy            2017-01-01T00:00:00  Unaffected
//   sony           2018-01-01T00:00:00  Unaffected
//   eopy           2017-01-01T00:00:00  Unaffected
//   eoy            2018-01-01T00:00:00  Unaffected
//   eony           2019-01-01T00:00:00  Unaffected
//   easter         2017-04-16T00:00:00
//   eastermonday   2017-04-16T00:00:00
//   ascension      2017-05-25T00:00:00
//   pentecost      2017-06-04T00:00:00
//   goodfriday     2017-04-14T00:00:00
//   midsommar      2017-06-24T00:00:00  midnight, 1st Saturday after 20th June
//   midsommarafton 2017-06-23T00:00:00  midnight, 1st Friday after 19th June
//   juhannus       2017-06-23T00:00:00  midnight, 1st Friday after 19th June
//
bool DatetimeParser::parse_named (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  // Experimental handling of date phrases, such as "first monday in march".
  // Note that this requires that phrases are delimited by EOS or WS.
  std::string token;
  std::vector <std::string> tokens;
  while (pig.getUntilWS (token))
  {
    tokens.push_back (token);
    if (! pig.skipWS ())
      break;
  }

/*
  // This group contains "1st monday ..." which must be processed before
  // initializeOrdinal below.
  if (initializeNthDayInMonth (tokens))
  {
    return true;
  }
*/

  // Restoration necessary because of the tokenization.
  pig.restoreTo (checkpoint);

  if (initializeNow            (pig) ||
      initializeLater          (pig) ||
      initializeSopd           (pig) ||
      initializeSod            (pig) ||
      initializeSond           (pig) ||
      initializeEopd           (pig) ||
      initializeEod            (pig) ||
      initializeEond           (pig) ||
      initializeSopw           (pig) ||
      initializeSow            (pig) ||
      initializeSonw           (pig) ||
      initializeEopw           (pig) ||
      initializeEow            (pig) ||
      initializeEonw           (pig) ||
      initializeSopww          (pig) ||  // Must appear after sopw
      initializeSonww          (pig) ||  // Must appear after sonw
      initializeSoww           (pig) ||  // Must appear after sow
      initializeEopww          (pig) ||  // Must appear after eopw
      initializeEonww          (pig) ||  // Must appear after eonw
      initializeEoww           (pig) ||  // Must appear after eow
      initializeSopm           (pig) ||
      initializeSom            (pig) ||
      initializeSonm           (pig) ||
      initializeEopm           (pig) ||
      initializeEom            (pig) ||
      initializeEonm           (pig) ||
      initializeSopq           (pig) ||
      initializeSoq            (pig) ||
      initializeSonq           (pig) ||
      initializeEopq           (pig) ||
      initializeEoq            (pig) ||
      initializeEonq           (pig) ||
      initializeSopy           (pig) ||
      initializeSoy            (pig) ||
      initializeSony           (pig) ||
      initializeEopy           (pig) ||
      initializeEoy            (pig) ||
      initializeEony           (pig) ||
      initializeInformalTime   (pig))
  {
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Valid epoch values are unsigned integers after 1980-01-01T00:00:00Z. This
// restriction means that '12' will not be identified as an epoch date.
bool DatetimeParser::parse_epoch (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  int epoch {};
  if (pig.getDigits (epoch)             &&
      ! unicodeLatinAlpha (pig.peek ()) &&
      epoch >= 315532800)
  {
    _date = static_cast <time_t> (epoch);
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// date_ext 'T' time_utc_ext 'Z'
// date_ext 'T' time_off_ext
// date_ext 'T' time_ext
bool DatetimeParser::parse_date_time_ext (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (parse_date_ext (pig) &&
      pig.skip ('T')       &&
      (parse_time_utc_ext (pig) ||
       parse_time_off_ext (pig) ||
       parse_time_ext     (pig)))
  {
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// YYYY-MM-DD
// YYYY-MM
// YYYY-DDD
// YYYY-Www-D
// YYYY-Www
bool DatetimeParser::parse_date_ext (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  int year {};
  if (parse_year (pig, year) &&
      pig.skip ('-'))
  {
    auto checkpointYear = pig.cursor ();

    int month {};
    int day {};
    int julian {};

    if (pig.skip ('W') &&
        parse_week (pig, _week))
    {
      if (pig.skip ('-') &&
          pig.getDigit (_weekday))
      {
        // What is happening here - must be something to do?
      }

      if (! unicodeLatinDigit (pig.peek ()))
      {
        _year = year;
        return true;
      }
    }

    pig.restoreTo (checkpointYear);

    if (parse_month (pig, month) &&
        pig.skip ('-')           &&
        parse_day (pig, day)     &&
        ! unicodeLatinDigit (pig.peek ()))
    {
      _year = year;
      _month = month;
      _day = day;
      return true;
    }

    pig.restoreTo (checkpointYear);

    if (parse_julian (pig, julian) &&
        ! unicodeLatinDigit (pig.peek ()))
    {
      _year = year;
      _julian = julian;
      return true;
    }

    pig.restoreTo (checkpointYear);

    if (parse_month (pig, month) &&
        pig.peek () != '-'       &&
        ! unicodeLatinDigit (pig.peek ()))
    {
      _year = year;
      _month = month;
      _day = 1;
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// ±hh[:mm]
bool DatetimeParser::parse_off_ext (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  int sign = pig.peek ();
  if (sign == '+' || sign == '-')
  {
    pig.skipN (1);

    int hour {0};
    int minute {0};

    if (parse_off_hour (pig, hour))
    {
      if (pig.skip (':'))
      {
        if (! parse_off_minute (pig, minute))
        {
          pig.restoreTo (checkpoint);
          return false;
        }
      }

      _offset = (hour * 3600) + (minute * 60);
      if (sign == '-')
        _offset = - _offset;

      if (! unicodeLatinDigit (pig.peek ()))
        return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// hh:mm[:ss]
bool DatetimeParser::parse_time_ext (Pig& pig, bool terminated)
{
  auto checkpoint = pig.cursor ();

  int hour {};
  int minute {};
  if (parse_hour (pig, hour) &&
      pig.skip (':')         &&
      parse_minute (pig, minute))
  {
    if (pig.skip (':'))
    {
      int second {};
      if (parse_second (pig, second) &&
          ! unicodeLatinDigit (pig.peek ()) &&
          (! terminated || (pig.peek () != '-' && pig.peek () != '+')))
      {
        _seconds = (hour * 3600) + (minute * 60) + second;
        return true;
      }

      pig.restoreTo (checkpoint);
      return false;
    }

    auto following = pig.peek ();
    if (! unicodeLatinDigit (following)    &&
        (! terminated || (following != '+' && following != '-')) &&
        following != 'A'                   &&
        following != 'a'                   &&
        following != 'P'                   &&
        following != 'p')
    {
      _seconds = (hour * 3600) + (minute * 60);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// time-ext 'Z'
bool DatetimeParser::parse_time_utc_ext (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (parse_time_ext (pig, false) &&
      pig.skip ('Z'))
  {
    if (! unicodeLatinDigit (pig.peek ()))
    {
      _utc = true;
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// time-ext off-ext
bool DatetimeParser::parse_time_off_ext (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (parse_time_ext (pig, false) &&
      parse_off_ext (pig))
  {
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// YYYYMMDDTHHMMSSZ
// YYYYMMDDTHHMMSS
bool DatetimeParser::parse_date_time (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (parse_date (pig) &&
      pig.skip ('T')   &&
      (parse_time_utc (pig) ||
       parse_time_off (pig) ||
       parse_time     (pig)))
  {
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// YYYYWww
// YYYYDDD
// YYYYMMDD
// YYYYMM
bool DatetimeParser::parse_date (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  int year {};
  int month {};
  int julian {};
  int week {};
  int weekday {};
  int day {};
  if (parse_year (pig, year))
  {
    auto checkpointYear = pig.cursor ();

    if (pig.skip ('W') &&
        parse_week (pig, week))
    {
      if (pig.getDigit (weekday))
        _weekday = weekday;

      if (! unicodeLatinDigit (pig.peek ()))
      {
        _year = year;
        _week = week;
        return true;
      }
    }

    pig.restoreTo (checkpointYear);

    if (parse_julian (pig, julian) &&
        ! unicodeLatinDigit (pig.peek ()))
    {
      _year = year;
      _julian = julian;
      return true;
    }

    pig.restoreTo (checkpointYear);

    if (parse_month (pig, month))
    {
      if (parse_day (pig, day))
      {
        if (! unicodeLatinDigit (pig.peek ()))
        {
          _year = year;
          _month = month;
          _day = day;
          return true;
        }
      }
      else
      {
        if (! unicodeLatinDigit (pig.peek ()))
        {
          _year = year;
          _month = month;
          _day = 1;
          return true;
        }
      }
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// <time> Z
bool DatetimeParser::parse_time_utc (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (parse_time (pig, false) &&
      pig.skip ('Z'))
  {
    _utc = true;
    if (! unicodeLatinDigit (pig.peek ()))
      return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// <time> <off>
bool DatetimeParser::parse_time_off (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (parse_time (pig, false) &&
      parse_off (pig))
  {
    auto terminator = pig.peek ();
    if (terminator != '-' && ! unicodeLatinDigit (terminator))
    {
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// hhmmss
// hhmm
bool DatetimeParser::parse_time (Pig& pig, bool terminated)
{
  auto checkpoint = pig.cursor ();

  int hour {};
  int minute {};
  if (parse_hour (pig, hour) &&
      parse_minute (pig, minute))
  {
    int second {};
    parse_second (pig, second);

    auto terminator = pig.peek ();
    if (! terminated ||
        (! unicodeLatinDigit (terminator) && terminator != '-' && terminator != '+'))
    {
      _seconds = (hour * 3600) + (minute * 60) + second;
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// ±hhmm
// ±hh
bool DatetimeParser::parse_off (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  int sign = pig.peek ();
  if (sign == '+' || sign == '-')
  {
    pig.skipN (1);

    int hour {};
    if (parse_off_hour (pig, hour))
    {
      int minute {};
      parse_off_minute (pig, minute);

      if (! unicodeLatinDigit (pig.peek ()))
      {
        _offset = (hour * 3600) + (minute * 60);
        if (sign == '-')
          _offset = - _offset;

        return true;
      }
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_year (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int year;
  if (pig.getDigit4 (year) &&
      year > 1969)
  {
    value = year;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_month (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int month;
  if (pig.getDigit2 (month) &&
      month > 0             &&
      month <= 12)
  {
    value = month;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_week (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int week;
  if (pig.getDigit2 (week) &&
      week > 0             &&
      week <= 53)
  {
    value = week;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_julian (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int julian;
  if (pig.getDigit3 (julian) &&
      julian > 0             &&
      julian <= 366)
  {
    value = julian;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_day (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int day;
  if (pig.getDigit2 (day) &&
      day > 0             &&
      day <= 31)
  {
    value = day;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_weekday (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int weekday;
  if (pig.getDigit (weekday) &&
      weekday >= 1           &&
      weekday <= 7)
  {
    value = weekday;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_hour (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int hour;
  if (pig.getDigit2 (hour) &&
      hour >= 0            &&
      hour < 24)
  {
    value = hour;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_minute (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int minute;
  if (pig.getDigit2 (minute) &&
      minute >= 0            &&
      minute < 60)
  {
    value = minute;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_second (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int second;
  if (pig.getDigit2 (second) &&
      second >= 0            &&
      second < 60)
  {
    value = second;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_off_hour (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int hour;
  if (pig.getDigit2 (hour) &&
      hour >= 0            &&
      hour <= 12)
  {
    value = hour;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::parse_off_minute (Pig& pig, int& value)
{
  auto checkpoint = pig.cursor ();

  int minute;
  if (pig.getDigit2 (minute) &&
      minute >= 0            &&
      minute < 60)
  {
    value = minute;
    return true;
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// now [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeNow (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("now"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      _date = time (nullptr);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// yesterday/abbrev  [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeYesterday (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  std::string token;
  if (pig.skipPartial ("yesterday", token) &&
      token.length () >= static_cast <std::string::size_type> (Datetime::minimumMatchLength))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      t->tm_mday -= 1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// today/abbrev  [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeToday (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  std::string token;
  if (pig.skipPartial ("today", token) &&
      token.length () >= static_cast <std::string::size_type> (Datetime::minimumMatchLength))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);

      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// tomorrow/abbrev  [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeTomorrow (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  std::string token;
  if (pig.skipPartial ("tomorrow", token) &&
      token.length () >= static_cast <std::string::size_type> (Datetime::minimumMatchLength))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday++;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// <digit>+ [ "st" | "nd" | "rd" | "th" ] [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeOrdinal (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  int number = 0;
  if (pig.getDigits (number) &&
      number > 0             &&
      number <= 31)
  {
    int character1;
    int character2;
    if (pig.getCharacter (character1)     &&
        pig.getCharacter (character2)     &&
        ! unicodeLatinAlpha (pig.peek ()) &&
        ! unicodeLatinDigit (pig.peek ()))
    {
      int remainder1 = number % 10;
      int remainder2 = number % 100;
      if ((remainder2 != 11 && remainder1 == 1 && character1 == 's' && character2 == 't') ||
          (remainder2 != 12 && remainder1 == 2 && character1 == 'n' && character2 == 'd') ||
          (remainder2 != 13 && remainder1 == 3 && character1 == 'r' && character2 == 'd') ||
          ((remainder2 == 11 ||
            remainder2 == 12 ||
            remainder2 == 13 ||
            remainder1 == 0 ||
            remainder1 > 3) && character1 == 't' && character2 == 'h'))
      {
        time_t now = time (nullptr);
        struct tm* t = localtime (&now);

        int y = t->tm_year + 1900;
        int m = t->tm_mon + 1;
        int d = t->tm_mday;

        if (Datetime::timeRelative && (1 <= number && number <= d))
        {
          if (++m > 12)
          {
            m = 1;
            y++;
          }
        }
        else if (!Datetime::timeRelative && (d < number && number <= Datetime::daysInMonth (y, m)))
        {
          if (--m < 1)
          {
            m = 12;
            y--;
          }
        }

        t->tm_hour = t->tm_min = t->tm_sec = 0;
        t->tm_mon  = m - 1;
        t->tm_mday = number;
        t->tm_year = y - 1900;
        t->tm_isdst = -1;

        _date = mktime (t);

        return true;
      }
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sunday/abbrev [ !<alpha> && !<digit> && !: && != ]
bool DatetimeParser::initializeDayName (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  std::string token;
  for (int day = 0; day <= 7; ++day)   // Deliberate <= so that 'sunday' is either 0 or 7.
  {
    if (pig.skipPartial (dayNames[day % 7], token) &&
        token.length () >= static_cast <std::string::size_type> (Datetime::minimumMatchLength))
    {
      auto following = pig.peek ();
      if (! unicodeLatinAlpha (following) &&
          ! unicodeLatinDigit (following) &&
          following != ':' &&
          following != '=')
      {
        time_t now = time (nullptr);
        struct tm* t = localtime (&now);

        if (t->tm_wday >= day)
        {
          t->tm_mday += day - t->tm_wday + (Datetime::timeRelative ? 7 : 0);
        }
        else
        {
          t->tm_mday += day - t->tm_wday - (Datetime::timeRelative ? 0 : 7);
        }

        t->tm_hour = t->tm_min = t->tm_sec = 0;
        t->tm_isdst = -1;
        _date = mktime (t);
        return true;
      }
    }

    pig.restoreTo (checkpoint);
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// january/abbrev [ !<alpha> && !<digit> && !: && != ]
bool DatetimeParser::initializeMonthName (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  std::string token;
  for (int month = 0; month < 12; ++month)
  {
    if (pig.skipPartial (monthNames[month], token) &&
        token.length () >= static_cast <std::string::size_type> (Datetime::minimumMatchLength))
    {
      auto following = pig.peek ();
      if (! unicodeLatinAlpha (following) &&
          ! unicodeLatinDigit (following) &&
          following != ':' &&
          following != '=')
      {
        time_t now = time (nullptr);
        struct tm* t = localtime (&now);

        if (t->tm_mon >= month && Datetime::timeRelative)
        {
          t->tm_year++;
        }

        t->tm_mon = month;
        t->tm_mday = 1;
        t->tm_hour = t->tm_min = t->tm_sec = 0;
        t->tm_isdst = -1;
        _date = mktime (t);
        return true;
      }
    }

    pig.restoreTo (checkpoint);
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// later/abbrev  [ !<alpha> && !<digit> ]
// someday/abbrev  [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeLater (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  std::string token;
  if ((pig.skipPartial ("later", token) &&
       token.length () >= static_cast <std::string::size_type> (Datetime::minimumMatchLength))

      ||

      (pig.skipPartial ("someday", token) &&
       token.length () >= static_cast <std::string::size_type> (std::max (Datetime::minimumMatchLength, 4))))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_year = 138;
      t->tm_mon = 0;
      t->tm_mday = 18;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sopd [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSopd (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sopd"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      t->tm_mday -= 1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sod [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSod (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sod"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sond [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSond (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sond"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday++;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eopd [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEopd (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eopd"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eod [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEod (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eod"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday++;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eond [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEond (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eond"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday += 2;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sopw [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSopw (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sopw"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);
      t->tm_hour = t->tm_min = t->tm_sec = 0;

      int extra = (t->tm_wday + 6) % 7;
      t->tm_mday -= extra;
      t->tm_mday -= 7;

      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sow [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSow (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sow"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);
      t->tm_hour = t->tm_min = t->tm_sec = 0;

      int extra = (t->tm_wday + 6) % 7;
      t->tm_mday -= extra;

      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sonw [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSonw (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sonw"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);
      t->tm_hour = t->tm_min = t->tm_sec = 0;

      int extra = (t->tm_wday + 6) % 7;
      t->tm_mday -= extra;
      t->tm_mday += 7;

      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eopw [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEopw (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eopw"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);
      t->tm_hour = t->tm_min = t->tm_sec = 0;

      int extra = (t->tm_wday + 6) % 7;
      t->tm_mday -= extra;

      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eow [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEow (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eow"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);
      t->tm_hour = t->tm_min = t->tm_sec = 0;

      int extra = (t->tm_wday + 6) % 7;
      t->tm_mday -= extra;
      t->tm_mday += 7;

      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eonw [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEonw (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eonw"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday += 15 - t->tm_wday;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sopww [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSopww (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sopww"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday += -6 - t->tm_wday;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// soww [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSoww (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("soww"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday += 8 - t->tm_wday;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sonww [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSonww (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sonww"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday += 8 - t->tm_wday;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eopww [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEopww (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eopww"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday -= (t->tm_wday + 1) % 7;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eoww [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEoww (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eoww"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday += 6 - t->tm_wday;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eonww [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEonww (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eonww"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mday += 13 - t->tm_wday;
      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sopm [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSopm (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sopm"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;

      if (t->tm_mon == 0)
      {
        t->tm_year--;
        t->tm_mon = 11;
      }
      else
        t->tm_mon--;

      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// som [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSom (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("som"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sonm [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSonm (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sonm"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;

      t->tm_mon++;
      if (t->tm_mon > 11)
      {
        t->tm_year++;
        t->tm_mon = 0;
      }

      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eopm [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEopm (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eopm"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eom [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEom (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eom"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;

      t->tm_mon++;
      if (t->tm_mon > 11)
      {
        t->tm_year++;
        t->tm_mon = 0;
      }

      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eonm [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEonm (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eonm"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mday = 1;
      t->tm_mon += 2;
      if (t->tm_mon > 11)
      {
        t->tm_year++;
        t->tm_mon -= 12;
      }

      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sopq [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSopq (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sopq"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mon -= t->tm_mon % 3;
      t->tm_mon -= 3;
      if (t->tm_mon < 0)
      {
        t->tm_mon += 12;
        t->tm_year--;
      }

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// soq [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSoq (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("soq"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon -= t->tm_mon % 3;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sonq [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSonq (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sonq"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mon += 3 - (t->tm_mon % 3);
      if (t->tm_mon > 11)
      {
        t->tm_mon -= 12;
        ++t->tm_year;
      }

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eopq [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEopq (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eopq"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon -= t->tm_mon % 3;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eoq [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEoq (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eoq"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_mon += 3 - (t->tm_mon % 3);
      if (t->tm_mon > 11)
      {
        t->tm_mon -= 12;
        ++t->tm_year;
      }

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eonq [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEonq (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eonq"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon += 6 - (t->tm_mon % 3);
      if (t->tm_mon > 11)
      {
        t->tm_mon -= 12;
        ++t->tm_year;
      }

      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sopy [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSopy (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sopy"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon = 0;
      t->tm_mday = 1;
      t->tm_year--;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// soy [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSoy (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("soy"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon = 0;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// sony [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeSony (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("sony"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon = 0;
      t->tm_mday = 1;
      t->tm_year++;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eopy [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEopy (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eopy"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon = 0;
      t->tm_mday = 1;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eoy [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEoy (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eoy"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon = 0;
      t->tm_mday = 1;
      t->tm_year++;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// eony [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEony (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("eony"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      t->tm_hour = t->tm_min = t->tm_sec = 0;
      t->tm_mon = 0;
      t->tm_mday = 1;
      t->tm_year += 2;
      t->tm_isdst = -1;
      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// easter       [ !<alpha> && !<digit> ]
// eastermonday [ !<alpha> && !<digit> ]
// ascension    [ !<alpha> && !<digit> ]
// pentecost    [ !<alpha> && !<digit> ]
// goodfriday   [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeEaster (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  std::vector <std::string> holidays = {"eastermonday", "easter", "ascension", "pentecost", "goodfriday"};
  std::vector <int>         offsets  = {             1,        0,          39,          49,           -2};

  std::string token;
  for (int holiday = 0; holiday < 5; ++holiday)
  {
    if (pig.skipLiteral (holidays[holiday]) &&
        ! unicodeLatinAlpha (pig.peek ()) &&
        ! unicodeLatinDigit (pig.peek ()))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      easter (t);
      _date = mktime (t);

      // If the result is earlier this year, then recalc for next year.
      if (_date < now)
      {
        t = localtime (&now);
        t->tm_year++;
        easter (t);
      }

      // Adjust according to holiday-specific offsets.
      t->tm_mday += offsets[holiday];

      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// midsommar [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeMidsommar (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("midsommar"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);
      midsommar (t);
      _date = mktime (t);

      // If the result is earlier this year, then recalc for next year.
      if (_date < now)
      {
        t = localtime (&now);
        t->tm_year++;
        midsommar (t);
      }

      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// midsommarafton [ !<alpha> && !<digit> ]
// juhannus [ !<alpha> && !<digit> ]
bool DatetimeParser::initializeMidsommarafton (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  if (pig.skipLiteral ("midsommarafton") ||
      pig.skipLiteral ("juhannus"))
  {
    auto following = pig.peek ();
    if (! unicodeLatinAlpha (following) &&
        ! unicodeLatinDigit (following))
    {
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);
      midsommarafton (t);
      _date = mktime (t);

      // If the result is earlier this year, then recalc for next year.
      if (_date < now)
      {
        t = localtime (&now);
        t->tm_year++;
        midsommarafton (t);
      }

      _date = mktime (t);
      return true;
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// 8am
// 8a
// 8:30am
// 8:30a
// 8:30
// 8:30:00
//
// \d+ [ : \d{2} ] [ am | a | pm | p ] [ !<alpha> && !<digit> && !: && !+ && !- ]
//
bool DatetimeParser::initializeInformalTime (Pig& pig)
{
  auto checkpoint = pig.cursor ();

  int digit = 0;
  bool needDesignator = true;   // Require am/pm.
  bool haveDesignator = false;  // Provided am/pm.
  if (pig.getDigit (digit))
  {
    int hours = digit;
    if (pig.getDigit (digit))
      hours = 10 * hours + digit;

    int minutes = 0;
    int seconds = 0;
    if (pig.skip (':'))
    {
      if (! pig.getDigit2 (minutes))
      {
        pig.restoreTo (checkpoint);
        return false;
      }

      if (pig.skip (':'))
      {
        if (! pig.getDigits (seconds))
        {
          pig.restoreTo (checkpoint);
          return false;
        }
      }

      needDesignator = false;
    }

    if (pig.skipLiteral ("am") ||
        pig.skipLiteral ("a"))
    {
      haveDesignator = true;
      if (hours == 12)
        hours = 0;
    }

    else if (pig.skipLiteral ("pm") ||
             pig.skipLiteral ("p"))
    {
      // Note: '12pm is an exception:
      //  12am = 0h
      //  11am = 11h + 12h
      //  12pm = 12h
      //  1pm  = 1h + 12h
      if (hours != 12)
        hours += 12;

      haveDesignator = true;
    }

    // Informal time needs to be terminated.
    auto following = pig.peek ();
    if (unicodeLatinAlpha (following) ||
        unicodeLatinDigit (following) ||
        following == ':'              ||
        following == '-'              ||
        following == '+')
    {
      pig.restoreTo (checkpoint);
      return false;
    }

    if (haveDesignator || ! needDesignator)
    {
      // Midnight today + hours:minutes:seconds.
      time_t now = time (nullptr);
      struct tm* t = localtime (&now);

      int now_seconds  = (t->tm_hour * 3600) + (t->tm_min * 60) + t->tm_sec;
      int calc_seconds = (hours      * 3600) + (minutes   * 60) + seconds;

      if (Datetime::timeRelative &&
          calc_seconds < now_seconds)
        ++t->tm_mday;

      // Basic validation.
      if (hours   >= 0 && hours   < 24 &&
          minutes >= 0 && minutes < 60 &&
          seconds >= 0 && seconds < 60)
      {
        t->tm_hour = hours;
        t->tm_min = minutes;
        t->tm_sec = seconds;
        t->tm_isdst = -1;
        _date = mktime (t);

        return true;
      }
    }
  }

  pig.restoreTo (checkpoint);
  return false;
}

////////////////////////////////////////////////////////////////////////////////
void DatetimeParser::easter (struct tm* t) const
{
  int Y = t->tm_year + 1900;
  int a = Y % 19;
  int b = Y / 100;
  int c = Y % 100;
  int d = b / 4;
  int e = b % 4;
  int f = (b + 8) / 25;
  int g = (b - f + 1) / 3;
  int h = (19 * a + b - d - g + 15) % 30;
  int i = c / 4;
  int k = c % 4;
  int L = (32 + 2 * e + 2 * i - h - k) % 7;
  int m = (a + 11 * h + 22 * L) / 451;
  int month = (h + L - 7 * m + 114) / 31;
  int day = ((h + L - 7 * m + 114) % 31) + 1;

  t->tm_isdst = -1;   // Requests that mktime determine summer time effect.
  t->tm_mday  = day;
  t->tm_mon   = month - 1;
  t->tm_year  = Y - 1900;
  t->tm_isdst = -1;
  t->tm_hour = t->tm_min = t->tm_sec = 0;
}

////////////////////////////////////////////////////////////////////////////////
void DatetimeParser::midsommar (struct tm* t) const
{
  t->tm_mon = 5;                          // June.
  t->tm_mday = 20;                        // Saturday after 20th.
  t->tm_hour = t->tm_min = t->tm_sec = 0; // Midnight.
  t->tm_isdst = -1;                       // Probably DST, but check.

  time_t then = mktime (t);               // Obtain the weekday of June 20th.
  struct tm* mid = localtime (&then);
  t->tm_mday += 6 - mid->tm_wday;         // How many days after 20th.
}

////////////////////////////////////////////////////////////////////////////////
void DatetimeParser::midsommarafton (struct tm* t) const
{
  t->tm_mon = 5;                          // June.
  t->tm_mday = 19;                        // Saturday after 20th.
  t->tm_hour = t->tm_min = t->tm_sec = 0; // Midnight.
  t->tm_isdst = -1;                       // Probably DST, but check.

  time_t then = mktime (t);               // Obtain the weekday of June 19th.
  struct tm* mid = localtime (&then);
  t->tm_mday += 5 - mid->tm_wday;         // How many days after 19th.
}

////////////////////////////////////////////////////////////////////////////////
// Suggested date expressions:
//   {ordinal} {day} in|of {month}
//   last|past|next|this {day}
//   last|past|next|this {month}
//   last|past|next|this week
//   last|past|next|this month
//   last|past|next|this weekend
//   last|past|next|this year
//   {day} last|past|next|this week
//   {day} [at] {time}
//   {time} {day}
//
// Candidates:
//   <dayname> <time>
//   <time>
//   tue 9am
//   Friday before easter
//   3 days before eom
//   in the morning
//   am|pm
//   4pm
//   noon
//   midnight
//   tomorrow in one year
//   in two weeks
//   2 weeks from now
//   2 weeks ago tuesday
//   thursday in 2 weeks
//   last day next month
//   10 days from today
//   thursday before last weekend in may
//   friday last full week in may
//   3rd wednesday this month
//   3 weeks after 2nd tuesday next month
//   100 days from the beginning of the month
//   10 days after last monday
//   sunday in the evening
//   in 6 hours
//   6 in the morning
//   kl 18
//   feb 11
//   11 feb
//   2011-02-08
//   11/19/2011
//   next business day
//   new moon
//   full moon
//   in 28 days
//   3rd quarter
//   week 23
//   {number} {unit}
//   - {number} {unit}
//   {ordinal} {unit} in {larger-unit}
//   end of day tomorrow
//   end of {day}
//   by {day}
//   first thing {day}
//

////////////////////////////////////////////////////////////////////////////////
// <ordinal> <weekday> in|of <month>
bool DatetimeParser::initializeNthDayInMonth (const std::vector <std::string>& tokens)
{
  if (tokens.size () == 4)
  {
    int ordinal {0};
    if (isOrdinal (tokens[0], ordinal))
    {
      auto day = Datetime::dayOfWeek (tokens[1]);
      if (day != -1)
      {
        if (tokens[2] == "in" ||
            tokens[2] == "of")
        {
          auto month = Datetime::monthOfYear (tokens[3]);
          if (month != -1)
          {
            std::cout << "# ordinal=" << ordinal << " day=" << day << " in month=" << month << '\n';

            // TODO Assume 1st of the month
            // TODO Assume current year
            // TODO Determine the day
            // TODO Project forwards/backwards, to the desired day
            // TODO Add ((ordinal - 1) * 7) days

            return true;
          }
        }
      }
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool DatetimeParser::isOrdinal (const std::string& token, int& ordinal)
{
  Pig p (token);
  int number;
  std::string suffix;
  if (p.getDigits (number) &&
      p.getRemainder (suffix))
  {
    if (((number >= 11 && number <= 13) && suffix == "th") ||
        (number % 10 == 1               && suffix == "st") ||
        (number % 10 == 2               && suffix == "nd") ||
        (number % 10 == 3               && suffix == "rd") ||
        (                                  suffix == "th"))
    {
      ordinal = number;
      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Validation via simple range checking.
bool DatetimeParser::validate ()
{
  // _year;
  if ((_year    && (_year    <   1900 || _year    >                                  2200)) ||
      (_month   && (_month   <      1 || _month   >                                    12)) ||
      (_week    && (_week    <      1 || _week    >                                    53)) ||
      (_weekday && (_weekday <      0 || _weekday >                                     6)) ||
      (_julian  && (_julian  <      1 || _julian  >          Datetime::daysInYear (_year))) ||
      (_day     && (_day     <      1 || _day     > Datetime::daysInMonth (_year, _month))) ||
      (_seconds && (_seconds <      1 || _seconds >                                 86400)) ||
      (_offset  && (_offset  < -86400 || _offset  >                                 86400)))
    return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////
// int tm_sec;       seconds (0 - 60)
// int tm_min;       minutes (0 - 59)
// int tm_hour;      hours (0 - 23)
// int tm_mday;      day of month (1 - 31)
// int tm_mon;       month of year (0 - 11)
// int tm_year;      year - 1900
// int tm_wday;      day of week (Sunday = 0)
// int tm_yday;      day of year (0 - 365)
// int tm_isdst;     is summer time in effect?
// char *tm_zone;    abbreviation of timezone name
// long tm_gmtoff;   offset from UTC in seconds
void DatetimeParser::resolve ()
{
  // Don't touch the original values.
  int year    = _year;
  int month   = _month;
  int week    = _week;
  int weekday = _weekday;
  int julian  = _julian;
  int day     = _day;
  int seconds = _seconds;
  int offset  = _offset;
  bool utc    = _utc;

  // Get current time.
  time_t now = time (nullptr);

  // A UTC offset needs to be accommodated.  Once the offset is subtracted,
  // only local and UTC times remain.
  if (offset)
  {
    seconds -= offset;
    now -= offset;
    utc = true;
  }

  // Get 'now' in the relevant location.
  struct tm* t_now = utc ? gmtime (&now) : localtime (&now);

  int seconds_now = (t_now->tm_hour * 3600) +
                    (t_now->tm_min  *   60) +
                    t_now->tm_sec;

  // Project forward one day if the specified seconds are earlier in the day
  // than the current seconds. Overridden by the ::timeRelative setting.
  if (Datetime::timeRelative &&
      year    == 0           &&
      month   == 0           &&
      day     == 0           &&
      week    == 0           &&
      weekday == 0           &&
      seconds < seconds_now)
  {
    seconds += 86400;
  }

  // Convert week + weekday --> julian.
  if (week)
  {
    julian = (week * 7) + weekday - Datetime::dayOfWeek (year, 1, 4) - 3;
  }

    // Provide default values for year, month, day.
  else
  {
    // Default values for year, month, day:
    //
    // y   m   d  -->  y   m   d
    // y   m   -  -->  y   m   1
    // y   -   -  -->  y   1   1
    // -   -   -  -->  now now now
    //
    if (year == 0)
    {
      year  = t_now->tm_year + 1900;
      month = t_now->tm_mon + 1;
      day   = t_now->tm_mday;
    }
    else
    {
      if (month == 0)
      {
        month = 1;
        day   = 1;
      }
      else if (day == 0)
        day = 1;
    }
  }

  if (julian)
  {
    month = 1;
    day = julian;
  }

  struct tm t {};
  t.tm_isdst = -1;  // Requests that mktime/gmtime determine summer time effect.
  t.tm_year = year - 1900;
  t.tm_mon = month - 1;
  t.tm_mday = day;

  if (seconds > 86400)
  {
    int days = seconds / 86400;
    t.tm_mday += days;
    seconds %= 86400;
  }

  t.tm_hour = seconds / 3600;
  t.tm_min = (seconds % 3600) / 60;
  t.tm_sec = seconds % 60;

  _date = utc ? timegm (&t) : mktime (&t);
}
