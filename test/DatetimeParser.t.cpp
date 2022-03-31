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

#include <Datetime.h>
#include <DatetimeParser.h>
#include <ctime>
#include <iostream>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
void testParseOpenRange (
  UnitTest& t,
  const std::string& input)
{
  std::string label = std::string ("DatetimeParser::parse_range (\"") + input + "\") --> ";

  DatetimeParser iso;

  auto range = iso.parse_range (input);

  t.ok (range.is_open (), label + "[start,...)");
}

////////////////////////////////////////////////////////////////////////////////
void testParseClosedRange (
  UnitTest& t,
  const std::string& input)
{
  std::string label = std::string ("DatetimeParser::parse_range (\"") + input + "\") --> ";

  DatetimeParser iso;

  auto range = iso.parse_range (input);

  t.ok (!range.is_open (), label + "[start,end)");
}

////////////////////////////////////////////////////////////////////////////////
void testParse (
  UnitTest& t,
  const std::string& input)
{
  std::string label = std::string ("DatetimeParser::parse_range positive '") + input + "' --> success";

  DatetimeParser positive;

  try
  {
    positive.parse_range (input);
    t.pass (label);
  }
  catch (const std::string& e)
  {
    t.fail ("exception thrown");
    t.diag (e);
  }
}

////////////////////////////////////////////////////////////////////////////////
void testParseError (
  UnitTest& t,
  const std::string& input)
{
  std::string label = std::string ("DatetimeParser::parse_range negative '") + input + "' --> fail";

  DatetimeParser neg;
  try
  {
    neg.parse_range (input);
    t.fail ("No exception thrown");
  }
  catch (const std::string& e)
  {
    t.pass (label);
  }
}

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (361);

  // Determine local and UTC time.
  time_t now = time (nullptr);
  struct tm* local_now = localtime (&now);
  int local_s = (local_now->tm_hour * 3600) +
                (local_now->tm_min  * 60)   +
                local_now->tm_sec;
  local_now->tm_hour  = 0;
  local_now->tm_min   = 0;
  local_now->tm_sec   = 0;
  local_now->tm_isdst = -1;
  time_t local = mktime (local_now);
  std::cout << "# local midnight today " << local << '\n';

  int year = 2013;
  int mo = 12;

  local_now->tm_year  = year - 1900;
  local_now->tm_mon   = mo - 1;
  local_now->tm_mday  = 6;
  local_now->tm_isdst = 0;
  time_t local6 = mktime (local_now);
  std::cout << "# local midnight 2013-12-06 " << local6 << '\n';

  local_now->tm_year  = year - 1900;
  local_now->tm_mon   = mo - 1;
  local_now->tm_mday  = 1;
  local_now->tm_isdst = 0;
  time_t local1 = mktime (local_now);
  std::cout << "# local midnight 2013-12-01 " << local1 << '\n';

  struct tm* utc_now = gmtime (&now);
  int utc_s = (utc_now->tm_hour * 3600) +
              (utc_now->tm_min  * 60)   +
              utc_now->tm_sec;
  utc_now->tm_hour  = 0;
  utc_now->tm_min   = 0;
  utc_now->tm_sec   = 0;
  utc_now->tm_isdst = -1;
  time_t utc = timegm (utc_now);
  std::cout << "# utc midnight today " << utc << '\n';

  utc_now->tm_year  = year - 1900;
  utc_now->tm_mon   = mo - 1;
  utc_now->tm_mday  = 6;
  utc_now->tm_isdst = 0;
  time_t utc6 = timegm (utc_now);
  std::cout << "# utc midnight 2013-12-06 " << utc6 << '\n';

  utc_now->tm_year  = year - 1900;
  utc_now->tm_mon   = mo - 1;
  utc_now->tm_mday  = 1;
  utc_now->tm_isdst = 0;
  time_t utc1 = timegm (utc_now);
  std::cout << "# utc midnight 2013-12-01 " << utc1 << '\n';

  int hms = (12 * 3600) + (34 * 60) + 56; // The time 12:34:56 in seconds.

  int ld = local_s > hms ? 86400 : 0;     // Local extra day if now > hms.
  int ud = utc_s   > hms ? 86400 : 0;     // UTC extra day if now > hms.
  std::cout << "# ld " << ld << '\n';
  std::cout << "# ud " << ud << '\n';

  // Aggregated
  testParseOpenRange(t, "12:34:56  ");

  // time-ext
  testParseOpenRange(t, "12:34:56Z");
  testParseOpenRange(t, "12:34Z");
  testParseOpenRange(t, "12:34:56+01:00");
  testParseOpenRange(t, "12:34:56+01");
  testParseOpenRange(t, "12:34+01:00");
  testParseOpenRange(t, "12:34+01");
  testParseOpenRange(t, "12:34:56");
  testParseOpenRange(t, "12:34");

  // datetime-ext
  testParseClosedRange(t, "2013-12-06");
  testParseClosedRange(t, "2013-340");
  testParseClosedRange(t, "2013-W49-5");
  testParseClosedRange(t, "2013-W49");
  testParseClosedRange(t, "2013-12");

  testParseOpenRange(t, "2013-12-06T12:34:56");
  testParseOpenRange(t, "2013-12-06T12:34");
  testParseOpenRange(t, "2013-340T12:34:56");
  testParseOpenRange(t, "2013-340T12:34");
  testParseOpenRange(t, "2013-W49-5T12:34:56");
  testParseOpenRange(t, "2013-W49-5T12:34");
  testParseOpenRange(t, "2013-W49T12:34:56");
  testParseOpenRange(t, "2013-W49T12:34");

  testParseOpenRange(t, "2013-12-06T12:34:56Z");
  testParseOpenRange(t, "2013-12-06T12:34Z");
  testParseOpenRange(t, "2013-340T12:34:56Z");
  testParseOpenRange(t, "2013-340T12:34Z");
  testParseOpenRange(t, "2013-W49-5T12:34:56Z");
  testParseOpenRange(t, "2013-W49-5T12:34Z");
  testParseOpenRange(t, "2013-W49T12:34:56Z");
  testParseOpenRange(t, "2013-W49T12:34Z");

  testParseOpenRange(t, "2013-12-06T12:34:56+01:00");
  testParseOpenRange(t, "2013-12-06T12:34:56+01");
  testParseOpenRange(t, "2013-12-06T12:34:56-01:00");
  testParseOpenRange(t, "2013-12-06T12:34:56-01");
  testParseOpenRange(t, "2013-12-06T12:34+01:00");
  testParseOpenRange(t, "2013-12-06T12:34+01");
  testParseOpenRange(t, "2013-12-06T12:34-01:00");
  testParseOpenRange(t, "2013-12-06T12:34-01");
  testParseOpenRange(t, "2013-340T12:34:56+01:00");
  testParseOpenRange(t, "2013-340T12:34:56+01");
  testParseOpenRange(t, "2013-340T12:34:56-01:00");
  testParseOpenRange(t, "2013-340T12:34:56-01");
  testParseOpenRange(t, "2013-340T12:34+01:00");
  testParseOpenRange(t, "2013-340T12:34+01");
  testParseOpenRange(t, "2013-340T12:34-01:00");
  testParseOpenRange(t, "2013-340T12:34-01");
  testParseOpenRange(t, "2013-W49-5T12:34:56+01:00");
  testParseOpenRange(t, "2013-W49-5T12:34:56+01");
  testParseOpenRange(t, "2013-W49-5T12:34:56-01:00");
  testParseOpenRange(t, "2013-W49-5T12:34:56-01");
  testParseOpenRange(t, "2013-W49-5T12:34+01:00");
  testParseOpenRange(t, "2013-W49-5T12:34+01");
  testParseOpenRange(t, "2013-W49-5T12:34-01:00");
  testParseOpenRange(t, "2013-W49-5T12:34-01");
  testParseOpenRange(t, "2013-W49T12:34:56+01:00");
  testParseOpenRange(t, "2013-W49T12:34:56+01");
  testParseOpenRange(t, "2013-W49T12:34:56-01:00");
  testParseOpenRange(t, "2013-W49T12:34:56-01");
  testParseOpenRange(t, "2013-W49T12:34+01:00");
  testParseOpenRange(t, "2013-W49T12:34+01");
  testParseOpenRange(t, "2013-W49T12:34-01:00");
  testParseOpenRange(t, "2013-W49T12:34-01");

  // The only non-extended forms.
  testParseOpenRange(t, "20131206T123456Z");
  testParseOpenRange(t, "20131206T123456");

  // Non-extended forms.

  // time
  testParseOpenRange(t, "123456Z");
  testParseOpenRange(t, "1234Z");
  testParseOpenRange(t, "123456+0100");
  testParseOpenRange(t, "123456+01");
  testParseOpenRange(t, "1234+0100");
  testParseOpenRange(t, "1234+01");
  testParseOpenRange(t, "123456");
  testParseOpenRange(t, "1234");

  // datetime
  testParseClosedRange(t, "20131206");
  testParseClosedRange(t, "2013340");
  testParseClosedRange(t, "2013W495");
  testParseClosedRange(t, "2013W49");
  testParseClosedRange(t, "201312");

  testParseOpenRange(t, "20131206T123456");
  testParseOpenRange(t, "20131206T1234");
  testParseOpenRange(t, "2013340T123456");
  testParseOpenRange(t, "2013340T1234");
  testParseOpenRange(t, "2013W495T123456");
  testParseOpenRange(t, "2013W495T1234");
  testParseOpenRange(t, "2013W49T123456");
  testParseOpenRange(t, "2013W49T1234");

  testParseOpenRange(t, "20131206T123456Z");
  testParseOpenRange(t, "20131206T1234Z");
  testParseOpenRange(t, "2013340T123456Z");
  testParseOpenRange(t, "2013340T1234Z");
  testParseOpenRange(t, "2013W495T123456Z");
  testParseOpenRange(t, "2013W495T1234Z");
  testParseOpenRange(t, "2013W49T123456Z");
  testParseOpenRange(t, "2013W49T1234Z");

  testParseOpenRange(t, "20131206T123456+0100");
  testParseOpenRange(t, "20131206T123456+01");
  testParseOpenRange(t, "20131206T123456-0100");
  testParseOpenRange(t, "20131206T123456-01");
  testParseOpenRange(t, "20131206T1234+0100");
  testParseOpenRange(t, "20131206T1234+01");
  testParseOpenRange(t, "20131206T1234-0100");
  testParseOpenRange(t, "20131206T1234-01");
  testParseOpenRange(t, "2013340T123456+0100");
  testParseOpenRange(t, "2013340T123456+01");
  testParseOpenRange(t, "2013340T123456-0100");
  testParseOpenRange(t, "2013340T123456-01");
  testParseOpenRange(t, "2013340T1234+0100");
  testParseOpenRange(t, "2013340T1234+01");
  testParseOpenRange(t, "2013340T1234-0100");
  testParseOpenRange(t, "2013340T1234-01");
  testParseOpenRange(t, "2013W495T123456+0100");
  testParseOpenRange(t, "2013W495T123456+01");
  testParseOpenRange(t, "2013W495T123456-0100");
  testParseOpenRange(t, "2013W495T123456-01");
  testParseOpenRange(t, "2013W495T1234+0100");
  testParseOpenRange(t, "2013W495T1234+01");
  testParseOpenRange(t, "2013W495T1234-0100");
  testParseOpenRange(t, "2013W495T1234-01");
  testParseOpenRange(t, "2013W49T123456+0100");
  testParseOpenRange(t, "2013W49T123456+01");
  testParseOpenRange(t, "2013W49T123456-0100");
  testParseOpenRange(t, "2013W49T123456-01");
  testParseOpenRange(t, "2013W49T1234+0100");
  testParseOpenRange(t, "2013W49T1234+01");
  testParseOpenRange(t, "2013W49T1234-0100");
  testParseOpenRange(t, "2013W49T1234-01");

  testParseOpenRange(t, "10:30am");
  testParseOpenRange(t, "8:30am");
  testParseOpenRange(t, "8:30a");
  testParseOpenRange(t, "8:30");
  testParseOpenRange(t, "8am");
  testParseOpenRange(t, "8a");
  testParseOpenRange(t, "8:30pm");
  testParseOpenRange(t, "8:30p");
  testParseOpenRange(t, "8pm");
  testParseOpenRange(t, "8p");
  testParseOpenRange(t, "12pm");
  testParseOpenRange(t, "1pm");

  // Names.
  testParseClosedRange (t, "yesterday");
  testParseClosedRange (t, "tomorrow");

  testParseClosedRange (t, "january");
  testParseClosedRange (t, "february");
  testParseClosedRange (t, "march");
  testParseClosedRange (t, "april");
  testParseClosedRange (t, "may");
  testParseClosedRange (t, "june");
  testParseClosedRange (t, "july");
  testParseClosedRange (t, "august");
  testParseClosedRange (t, "september");
  testParseClosedRange (t, "october");
  testParseClosedRange (t, "november");
  testParseClosedRange (t, "december");

  testParseClosedRange (t, "jan");
  testParseClosedRange (t, "feb");
  testParseClosedRange (t, "mar");
  testParseClosedRange (t, "apr");
  testParseClosedRange (t, "may");
  testParseClosedRange (t, "jun");
  testParseClosedRange (t, "jul");
  testParseClosedRange (t, "aug");
  testParseClosedRange (t, "sep");
  testParseClosedRange (t, "oct");
  testParseClosedRange (t, "nov");
  testParseClosedRange (t, "dec");

  testParseClosedRange (t, "sunday");
  testParseClosedRange (t, "monday");
  testParseClosedRange (t, "tuesday");
  testParseClosedRange (t, "wednesday");
  testParseClosedRange (t, "thursday");
  testParseClosedRange (t, "friday");
  testParseClosedRange (t, "saturday");

  testParseClosedRange (t, "sun");
  testParseClosedRange (t, "mon");
  testParseClosedRange (t, "tue");
  testParseClosedRange (t, "wed");
  testParseClosedRange (t, "thu");
  testParseClosedRange (t, "fri");
  testParseClosedRange (t, "sat");

  Datetime r11 ("eow");
  Datetime r16 ("sonw");
  Datetime r23 ("sow");
  Datetime r17 ("sonm");
  Datetime r18 ("som");
  Datetime r19 ("sony");
  Datetime r19a ("soy");
  Datetime r19b ("eoy");
  Datetime r19c ("soq");
  Datetime r19d ("eoq");

  Datetime first ("1st");
  Datetime second ("2nd");
  Datetime third ("3rd");
  Datetime fourth ("4th");

  Datetime later ("later");

  // Test all format options.
  Datetime r32 ("2015-10-28T12:55:00");

  // Test all parse options.
  Datetime r33 ("2015 10 28 19 28 01", "Y M D H N S");
  t.is(r33.year (),   2015,         "Y works");
  t.is(r33.month (),    10,         "M works");
  t.is(r33.day (),      28,         "D works");
  t.is(r33.hour (),     19,         "H works");
  t.is(r33.minute (),   28,         "N works");
  t.is(r33.second (),    1,         "S works");

  Datetime r34 ("15 5 4 3 2 1", "y m d h n s");
  t.is(r34.year (),   2015,         "y works");
  t.is(r34.month (),     5,         "m works");
  t.is(r34.day (),       4,         "d works");
  t.is(r34.hour (),      3,         "h works");
  t.is(r34.minute (),    2,         "n works");
  t.is(r34.second (),    1,         "s works");

  Datetime r35 ("Wednesday October 28 2015", "A B D Y");
  t.is(r35.year (),    2015,        "Y works");
  t.is(r35.month (),     10,        "B works");
  t.is(r35.day (),       28,        "D works");
  t.is(r35.dayOfWeek (),  3,        "A works");

  Datetime r36 ("Wed Oct 28 15", "a b d y");
  t.is(r36.year (),    2015,        "y works");
  t.is(r36.month (),     10,        "b works");
  t.is(r36.day (),       28,        "d works");
  t.is(r36.dayOfWeek (),  3,        "a works");

  Datetime r37 ("19th");
  t.is (r37.day (),      19,        "'19th' --> 19");

  // Embedded parsing.
  testParseError (t, "nowadays");
  testParse (t, "now+1d");
  testParse (t, "now-1d");
  testParseOpenRange (t, "now)");
  testParseError (t, "now7");
  testParseError (t, "tomorrov");

  testParseError (t, "yesteryear");
  testParseClosedRange (t, "yest+1d");
  testParseClosedRange (t, "yest-1d");
  testParseClosedRange (t, "yest)");
  testParseError (t, "yest7");
  testParseClosedRange (t, "yesterday");

  testParse      (t, "1234567890+0");
  testParse      (t, "1234567890-0");
  testParse      (t, "1234567890)");

  // Negative tests, all expected to fail.
  testParseError (t, "");
  testParseError (t, "foo");
  testParseError (t, "-2014-07-07");
  testParseError (t, "2014-07-");
  testParseError (t, "2014-0-12");
  testParseError (t, "abcd-ab-ab");
  testParseError (t, "2014-000");
  testParseClosedRange (t, "2014-001");
  testParseClosedRange (t, "2014-365");
  testParseError (t, "2014-366");
  testParseError (t, "2014-367");
  testParseError (t, "2014-999");
  testParseError (t, "2014-999999999");
  testParseError (t, "2014-W00");
  testParseError (t, "2014-W54");
  testParseError (t, "2014-W240");
  testParseError (t, "2014-W248");
  testParseError (t, "2014-W24200");
  //testParseError (t, "2014-00");        // Looks like Datetime::parse_time_off 'hhmm-hh'
  testParseError (t, "2014-13");
  testParseError (t, "2014-99");
  testParseError (t, "25:00");
  testParseError (t, "99:00");
  testParseError (t, "12:60");
  testParseError (t, "12:99");
  testParseError (t, "12:ab");
  testParseError (t, "ab:12");
  testParseError (t, "ab:cd");
  testParseError (t, "-12:12");
  testParseError (t, "12:-12");
  testParseError (t, "25:00Z");
  testParseError (t, "99:00Z");
  testParseError (t, "12:60Z");
  testParseError (t, "12:99Z");
  testParseError (t, "12:abZ");
  testParseError (t, "ab:12Z");
  testParseError (t, "ab:cdZ");
  testParseError (t, "-12:12Z");
  testParseError (t, "12:-12Z");
  testParseError (t, "25:00+01:00");
  testParseError (t, "99:00+01:00");
  testParseError (t, "12:60+01:00");
  testParseError (t, "12:99+01:00");
  testParseError (t, "12:ab+01:00");
  testParseError (t, "ab:12+01:00");
  testParseError (t, "ab:cd+01:00");
  testParseError (t, "-12:12+01:00");
  testParseError (t, "12:-12+01:00");
  testParseError (t, "25:00-01:00");
  testParseError (t, "99:00-01:00");
  testParseError (t, "12:60-01:00");
  testParseError (t, "12:99-01:00");
  testParseError (t, "12:ab-01:00");
  testParseError (t, "ab:12-01:00");
  testParseError (t, "ab:cd-01:00");
  testParseError (t, "-12:12-01:00");
  testParseError (t, "12:-12-01:00");
  testParseError (t, "25:00:00");
  testParseError (t, "99:00:00");
  testParseError (t, "12:60:00");
  testParseError (t, "12:99:00");
  testParseError (t, "12:12:60");
  testParseError (t, "12:12:99");
  testParseError (t, "12:ab:00");
  testParseError (t, "ab:12:00");
  testParseError (t, "12:12:ab");
  testParseError (t, "ab:cd:ef");
  testParseError (t, "-12:12:12");
  testParseError (t, "12:-12:12");
  testParseError (t, "12:12:-12");
  testParseError (t, "25:00:00Z");
  testParseError (t, "99:00:00Z");
  testParseError (t, "12:60:00Z");
  testParseError (t, "12:99:00Z");
  testParseError (t, "12:12:60Z");
  testParseError (t, "12:12:99Z");
  testParseError (t, "12:ab:00Z");
  testParseError (t, "ab:12:00Z");
  testParseError (t, "12:12:abZ");
  testParseError (t, "ab:cd:efZ");
  testParseError (t, "-12:12:12Z");
  testParseError (t, "12:-12:12Z");
  testParseError (t, "12:12:-12Z");
  testParseError (t, "25:00:00+01:00");
  testParseError (t, "95:00:00+01:00");
  testParseError (t, "12:60:00+01:00");
  testParseError (t, "12:99:00+01:00");
  testParseError (t, "12:12:60+01:00");
  testParseError (t, "12:12:99+01:00");
  testParseError (t, "12:ab:00+01:00");
  testParseError (t, "ab:12:00+01:00");
  testParseError (t, "12:12:ab+01:00");
  testParseError (t, "ab:cd:ef+01:00");
  testParseError (t, "-12:12:12+01:00");
  testParseError (t, "12:-12:12+01:00");
  testParseError (t, "12:12:-12+01:00");
  testParseError (t, "25:00:00-01:00");
  testParseError (t, "95:00:00-01:00");
  testParseError (t, "12:60:00-01:00");
  testParseError (t, "12:99:00-01:00");
  testParseError (t, "12:12:60-01:00");
  testParseError (t, "12:12:99-01:00");
  testParseError (t, "12:ab:00-01:00");
  testParseError (t, "ab:12:00-01:00");
  testParseError (t, "12:12:ab-01:00");
  testParseError (t, "ab:cd:ef-01:00");
  testParseError (t, "-12:12:12-01:00");
  testParseError (t, "12:-12:12-01:00");
  testParseError (t, "12:12:-12-01:00");
  testParseError (t, "12:12:12-13:00");
  testParseError (t, "12:12:12-24:00");
  testParseError (t, "12:12:12-99:00");
  testParseError (t, "12:12:12-03:60");
  testParseError (t, "12:12:12-03:99");
  testParseError (t, "12:12:12-3:20");
  testParseError (t, "12:12:12-03:2");
  testParseError (t, "12:12:12-3:2");
  testParseError (t, "12:12:12+13:00");
  testParseError (t, "12:12:12+24:00");
  testParseError (t, "12:12:12+99:00");
  testParseError (t, "12:12:12+03:60");
  testParseError (t, "12:12:12+03:99");
  testParseError (t, "12:12:12+3:20");
  testParseError (t, "12:12:12+03:2");
  testParseError (t, "12:12:12+3:2");
  testParseError (t, "12:12-13:00");
  testParseError (t, "12:12-24:00");
  testParseError (t, "12:12-99:00");
  testParseError (t, "12:12-03:60");
  testParseError (t, "12:12-03:99");
  testParseError (t, "12:12-3:20");
  testParseError (t, "12:12-03:2");
  testParseError (t, "12:12-3:2");
  testParseError (t, "12:12+13:00");
  testParseError (t, "12:12+24:00");
  testParseError (t, "12:12+99:00");
  testParseError (t, "12:12+03:60");
  testParseError (t, "12:12+03:99");
  testParseError (t, "12:12+3:20");
  testParseError (t, "12:12+03:2");
  testParseError (t, "12:12+3:2");

  // Test with standalone date enable/disabled.
  Datetime::standaloneDateEnabled = true;
  testParseClosedRange(t, "20170319");
  Datetime::standaloneDateEnabled = false;
  testParseError (t, "20170319");
  Datetime::standaloneDateEnabled = true;

  Datetime::standaloneTimeEnabled = true;
  testParseOpenRange(t, "235959");
  Datetime::standaloneTimeEnabled = false;
  testParseError (t, "235959");
  Datetime::standaloneTimeEnabled = true;

  // Weekdays and month names can no longer be followed by ':' or '='.
  testParseClosedRange(t, "jan");
  testParseError (t, "jan:");
  testParseClosedRange(t, "mon");
  testParseError (t, "mon:");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
