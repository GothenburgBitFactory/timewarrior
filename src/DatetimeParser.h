////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_DATETIMEPARSER
#define INCLUDED_DATETIMEPARSER

#include <string>
#include <ctime>
#include <Pig.h>
#include <Range.h>

class DatetimeParser
{
public:
  DatetimeParser () = default;
  Range parse_range(const std::string&);

private:
  void clear ();

  bool parse_named         (Pig&);
  bool parse_named_day     (Pig&);
  bool parse_named_month   (Pig&);
  bool parse_epoch         (Pig&);
  bool parse_date_time_ext (Pig&);
  bool parse_date_ext      (Pig&);
  bool parse_off_ext       (Pig&);
  bool parse_time_ext      (Pig&, bool terminated = true);
  bool parse_time_utc_ext  (Pig&);
  bool parse_time_off_ext  (Pig&);
  bool parse_date_time     (Pig&);
  bool parse_date          (Pig&);
  bool parse_time_utc      (Pig&);
  bool parse_time_off      (Pig&);
  bool parse_time          (Pig&, bool terminated = true);
  bool parse_informal_time (Pig&);
  bool parse_off           (Pig&);

  bool parse_year          (Pig&, int&);
  bool parse_month         (Pig&, int&);
  bool parse_week          (Pig&, int&);
  bool parse_julian        (Pig&, int&);
  bool parse_day           (Pig&, int&);
  bool parse_weekday       (Pig&, int&);
  bool parse_hour          (Pig&, int&);
  bool parse_minute        (Pig&, int&);
  bool parse_second        (Pig&, int&);
  bool parse_off_hour      (Pig&, int&);
  bool parse_off_minute    (Pig&, int&);

  bool initializeNow            (Pig&);
  bool initializeYesterday      (Pig&);
  bool initializeToday          (Pig&);
  bool initializeTomorrow       (Pig&);
  bool initializeOrdinal        (Pig&);
  bool initializeDayName        (Pig&);
  bool initializeMonthName      (Pig&);
  bool initializeLater          (Pig&);
  bool initializeSopd           (Pig&);
  bool initializeSod            (Pig&);
  bool initializeSond           (Pig&);
  bool initializeEopd           (Pig&);
  bool initializeEod            (Pig&);
  bool initializeEond           (Pig&);
  bool initializeSopw           (Pig&);
  bool initializeSow            (Pig&);
  bool initializeSonw           (Pig&);
  bool initializeEopw           (Pig&);
  bool initializeEow            (Pig&);
  bool initializeEonw           (Pig&);
  bool initializeSopww          (Pig&);
  bool initializeSonww          (Pig&);
  bool initializeSoww           (Pig&);
  bool initializeEopww          (Pig&);
  bool initializeEonww          (Pig&);
  bool initializeEoww           (Pig&);
  bool initializeSopm           (Pig&);
  bool initializeSom            (Pig&);
  bool initializeSonm           (Pig&);
  bool initializeEopm           (Pig&);
  bool initializeEom            (Pig&);
  bool initializeEonm           (Pig&);
  bool initializeSopq           (Pig&);
  bool initializeSoq            (Pig&);
  bool initializeSonq           (Pig&);
  bool initializeEopq           (Pig&);
  bool initializeEoq            (Pig&);
  bool initializeEonq           (Pig&);
  bool initializeSopy           (Pig&);
  bool initializeSoy            (Pig&);
  bool initializeSony           (Pig&);
  bool initializeEopy           (Pig&);
  bool initializeEoy            (Pig&);
  bool initializeEony           (Pig&);
  bool initializeEaster         (Pig&);
  bool initializeMidsommar      (Pig&);
  bool initializeMidsommarafton (Pig&);
  bool initializeInformalTime   (Pig&);

  void easter (struct tm*) const;
  void midsommar (struct tm*) const;
  void midsommarafton (struct tm*) const;

  bool initializeNthDayInMonth  (const std::vector <std::string>&);

  bool isOrdinal (const std::string&, int&);

  bool validate ();
  void resolve ();

public:
  int _year    {0};
  int _month   {0};
  int _week    {0};
  int _weekday {0};
  int _julian  {0};
  int _day     {0};
  int _seconds {0};
  int _offset  {0};
  bool _utc    {false};
  time_t _date {0};
};


#endif
