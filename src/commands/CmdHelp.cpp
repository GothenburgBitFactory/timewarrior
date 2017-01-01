////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2017, Paul Beckingham, Federico Hernandez.
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
#include <commands.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdHelpUsage (const Extensions& extensions)
{
  // TODO This is going to need more formatting.
  // TODO Align the arg types?
  std::cout << '\n'
            << "Usage: timew [--version]\n"
            << "       timew cancel\n"
            << "       timew config [<name> [<value | '']]\n"
            << "       timew continue\n"
            << "       timew day [<interval>] [<tag> ...]\n"
            << "       timew delete @<id> [@<id> ...]\n"
            << "       timew diagnostics\n"
            << "       timew export [<interval>] [<tag> ...]\n"
            << "       timew extensions\n"
/*
            << "       timew fill @<id> [@<id> ...]\n"
*/
            << "       timew gaps [<interval>] [<tag> ...]\n"
            << "       timew get <DOM> [<DOM> ...]\n"
            << "       timew help [<command> | interval | hints | date | duration]\n"
            << "       timew join @<id> @<id>\n"
            << "       timew lengthen @<id> [@<id> ...] <duration>\n"
            << "       timew month [<interval>] [<tag> ...]\n"
            << "       timew move @<id> <date>\n"
            << "       timew [report] <report> [<interval>] [<tag> ...]\n"
            << "       timew shorten @<id> [@<id> ...] <duration>\n"
            << "       timew show\n"
            << "       timew split @<id> [@<id> ...]\n"
            << "       timew start [<date>] [<tag> ...]\n"
            << "       timew stop [<tag> ...]\n"
            << "       timew summary [<interval>] [<tag> ...]\n"
            << "       timew tag @<id> [@<id> ...] <tag> [<tag> ...]\n"
            << "       timew tags\n"
            << "       timew track <interval> [<tag> ...]\n"
            << "       timew untag @<id> [@<id> ...] <tag> [<tag> ...]\n"
            << "       timew week [<interval>] [<tag> ...]\n"
            << '\n';

  if (extensions.all ().size ())
  {
    std::cout << "Extensions (extensions do not provide help):\n";

    for (auto& ext : extensions.all ())
      std::cout << "       " << File (ext).name () << '\n';

    std::cout << '\n';
  }

  std::cout << "Additional help:\n"
            << "       timew help <command>\n"
            << "       timew help interval\n"
            << "       timew help hints\n"
            << "       timew help date\n"
            << "       timew help duration\n"
            << "       timew help dom\n"
            << '\n'
            << "Interval:\n"
            << "       [from] <date>\n"
            << "       [from] <date> to/- <date>\n"
            << "       [from] <date> for <duration>\n"
            << "       <duration> before/after <date>\n"
            << "       <duration> ago\n"
            << "       [for] <duration>\n"
            << '\n'
            << "Tag:\n"
            << "       Word\n"
            << "       'Single Quoted Words'\n"
            << "       \"Double Quoted Words\"\n"
            << "       Escaped\\ Spaces\n"
            << '\n'
            << "Configuration overrides:\n"
            << "       rc.<name>=<value>\n"
            << '\n';

  // TODO List all extensions.

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TODO Should the CmdXxx functions themselves be responsible for providing both
//      the command syntax and the help text?
//      A: Only if they are upgraded to objects.
//
// Strict 80-character limit.
// Provide examples where appropriate - enough to cover all uses.
int CmdHelp (
  const CLI& cli,
  const Extensions& extensions)
{
  auto words = cli.getWords ();
  if (words.size ())
  {
    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    if (words[0] == "cancel")
      std::cout << '\n'
                << "Syntax: timew cancel\n"
                << '\n'
                << "If there is an open interval, it is abandoned.\n"
                << '\n'
                << "See also 'start', 'stop', 'delete'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "config")
      std::cout << '\n'
                << "Syntax: timew config [<name> [<value> | '']]\n"
                << '\n'
                << "Allows setting and removing configuration values, as an alternative to directly\n"
                << "editing your ~/.timewarrior/timewarrior.cfg file. For example:\n"
                << '\n'
                << "  $ timew config verbose yes\n"
                << "  $ timew config verbose ''\n"
                << "  $ timew config verbose\n"
                << '\n'
                << "The first command sets 'verbose' to 'yes'. The second sets it to a blank value\n"
                << "which overrides the default value. The third example deletes the 'verbose'\n"
                << "setting.\n"
                << '\n'
                << "When modifying configuration in this way, interactive confirmation will be\n"
                << "sought. To override this confirmation, use the ':yes' hint, which means you\n"
                << "intend to answer 'yes' to the confirmation questions:\n"
                << '\n'
                << "  $ timew config verbose '' :yes\n"
                << '\n'
                << "If no arguments are provided, all configuration settings are shown:\n"
                << '\n'
                << "  $ timew config\n"
                << "  verbose = yes\n"
                << "  ...\n"
                << '\n'
                << "See also 'hints', 'show'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "continue")
      std::cout << '\n'
                << "Syntax: timew continue\n"
                << '\n'
                << "Resumes tracking the most recently closed interval. For exqample:\n"
                << '\n'
                << "  $ timew track 9am - 10am tag1 tag2\n"
                << "  $ timew continue\n"
                << '\n'
                << "The 'continue' command creates a new interval, starting now, and using the tags\n"
                << "'tag1' and 'tag2'.\n"
                << '\n'
                << "This command is a convenient way to resume work without re-entering the tags.\n"
                << '\n'
                << "See also 'start', 'stop'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "date")
      std::cout << '\n'
                << "Timewarrior supports the following date formats based on ISO-8601:\n"
                << '\n'
                << "  <extended-date> [T <extended-time>]   Extended date, optional extended time\n"
                << "  <date> [T <time>]                     Date, optional time\n"
                << "  <extended-time>                       Extended time\n"
                << "  <time>                                Time\n"
                << '\n'
                << "  extended-date:\n"
                << "    YYYY-MM-DD                          Year, month, day\n"
                << "    YYYY-MM                             Year, month, 1st\n"
                << "    YYYY-DDD                            Year, Julian day 001-366\n"
                << "    YYYY-WwwD                           Year, week number, day number\n"
                << "    YYYY-Www                            Year, week number, day 1\n"
                << '\n'
                << "  extended-time:\n"
                << "    hh:mm[:ss]Z                         Hours, minutes, optional seconds, UTC\n"
                << "    hh:mm[:ss][+/-hh:mm]                Hours, minutes, optional seconds, TZ\n"
                << '\n'
                << "  date:\n"
                << "    YYYYMMDD                            Year, month, day\n"
                << "    YYYYWww                             Year, week number, day number\n"
                << "    YYYYDDD                             Year, Julian day 001-366\n"
                << '\n'
                << "  time:\n"
                << "    hhmm[ss]Z                           Hour, minutes, optional seconds, UTC\n"
                << "    hhmm[ss][+/-hh[mm]]                 Hour, minutes, optional seconds, TZ\n"
                << '\n'
                << "Examples:\n"
                << "  2016-06-09T08:12:00Z\n"
                << "  2016-06T08:12:00+01:00\n"
                << "  2016-06T08:12Z\n"
                << "  2016-161\n"
                << "  2016-W244\n"
                << "  2016-W24\n"
                << "  20160609T081200Z\n"
                << "  2016W24\n"
                << "  8:12:00Z\n"
                << "  0812-0500\n"
                << '\n'
                << "In addition to the standard date formats, the following are supported:\n"
                << '\n'
                << "  now                                   Current date and time\n"
                << "  today                                 Current date at 0:00:00\n"
                << "  sod, eod                              Current date at 0:00:00 and 24:00:00\n"
                << "  yesterday                             Yesterday at 0:00:00\n"
                << "  tomorrow                              Tomorrow at 0:00:00 (midnight tonight)\n"
                << "  <day-of-week>                         Previous named day at 0:00:00\n"
                << "  <month-of-year>                       Previous 1st of the  month at 0:00:00\n"
                << "  hh:mm[:ss][am|a|pm|p]                 Short time format\n"
                << "  Nst, Nnd, Nrd, Nth                    Previous 1st, 2nd, 3rd ...\n"
                << "  <epoch>                               POSIX time\n"
                << "  later                                 2038-01-18T0:00:00 (Y2K38)\n"
                << "  someday                               2038-01-18T0:00:00 (Y2K38)\n"
                << "  soy, eoy                              Previous start/end of year\n"
                << "  socy, eocy                            Start/end of current year\n"
                << "  soq, eoq                              Previous start/end of quarter\n"
                << "  socq, eocq                            Start/end of current quarter\n"
                << "  som, eom                              Previous start/end of month\n"
                << "  socm, eocm                            Start/end of current month\n"
                << "  sow, eow                              Previous start/end of week\n"
                << "  socw, eocw                            Start/end of current week\n"
                << "  soww, eoww                            Start/end of current work week (mon - fri)\n"
                << "  easter                                Easter Sunday\n"
                << "  eastermonday                          Easter Monday\n"
                << "  ascension                             Ascension\n"
                << "  pentecost                             Pentecost\n"
                << "  goodfriday                            Good Friday\n"
                << "  midsommar                             midnight, 1st Saturday after 20th June\n"
                << "  midsommarafton                        midnight, 1st Friday after 19th June\n"
                << "  juhannus                              midnight, 1st Friday after 19th June\n"
                << '\n'
                << "Examples:\n"
                << "  8am\n"
                << "  24th\n"
                << "  monday\n"
                << "  august\n"
                << '\n'
                << "See also 'duration', 'hints'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "duration")
      std::cout << '\n'
                << "Timewarrior supports the following duration formats based on ISO-8601:\n"
                << '\n'
                << "  'P' [nn 'Y'] [nn 'M'] [nn 'D'] ['T' [nn 'H'] [nn 'M'] [nn 'S']]\n"
                << "  PnnW\n"
                << '\n'
                << "Examples:\n"
                << "  P1Y           1 year\n"
                << "  P1.5M         1.5 months\n"
                << "  PT1S          1 second\n"
                << "  PT4.5H        4.5 hours\n"
                << "  PT4H30M       4.5 hours\n"
                << "  P600D         600 days\n"
                << "  P3W           3 weeks\n"
                << "  P1Y1DT1H1M1S  1 year and 25 hours, 61 seconds (imprecise term)\n"
                << '\n'
                << "  Note that the year and month terms are imprecise, being defined as 365d and\n"
                << "  30d respectively. For precision use the other terms.\n"
                << '\n'
                << "In addition to the standard duration formats, the following are supported:\n"
                << '\n'
                << "  n[.n]<unit>\n"
                << '\n'
                << "Where the <unit> is one of:\n"
                << '\n'
                << "  annual\n"
                << "  biannual\n"
                << "  bimonthly\n"
                << "  biweekly\n"
                << "  biyearly\n"
                << "  daily\n"
                << "  days, day, d\n"
                << "  fortnight\n"
                << "  hours, hour, hrs, hr, h\n"
                << "  minutes, minute, mins, min\n"
                << "  monthly, months, month, mnths, mths, mth, mos, mo, m\n"
                << "  quarterly, quarters, quarter, qrtrs, qtr, q\n"
                << "  semiannual\n"
                << "  sennight\n"
                << "  seconds, second, secs, sec, s\n"
                << "  weekdays\n"
                << "  weekly, weeks, week, wks, wk, w\n"
                << "  yearly, years, year, yrs, yr, y\n"
                << '\n'
                << "Examples:\n"
                << "  1hour         60 minutes\n"
                << "  1.5h          90 minutes\n"
                << "  3mo           3 months\n"
                << "  10d           10 days\n"
                << '\n'
                << "  Note that the year, quarter and month terms are imprecise, being defined as\n"
                << "  365d, 91d and 30d respectively. For precision use the other terms.\n"
                << '\n'
                << "See also 'date', 'hints'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "day")
      std::cout << '\n'
                << "Syntax: timew day [<interval>] [<tag> ...]\n"
                << '\n'
                << "The day command shows a chart depicting a single day (today by default), with\n"
                << "colored blocks drawn on a timeline. The chart summarizes the tracked and\n"
                << "untracked time.\n"
                << '\n'
                << "Accepts date ranges and tags for filtering, or shortcut hints:\n"
                << '\n'
                << "  $ timew day monday - today\n"
                << "  $ timew day :week\n"
                << "  $ timew day :month\n"
                << '\n'
                << "The 'reports.day.range' configuration setting overrides the default date range.\n"
                << "The default date range shown is ':day'.\n"
                << '\n'
                << "The ':blank' hint causes only the excluded time to be shown, with no tracked\n"
                << "time. This can be used to see the exclusions.\n"
                << '\n'
                << "For more details, and precise times, use the 'summary' report.\n"
                << '\n'
                << "See also 'week', 'month', 'summary'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "delete")
      std::cout << '\n'
                << "Syntax: timew delete @<id> [@<id> ...]\n"
                << '\n'
                << "Deletes an interval. Using the 'summary' command, and specifying the ':ids' hint\n"
                << "shows interval IDs. Using the right ID, you can identify an interval to delete.\n"
                << "For example, show the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to delete:\n"
                << '\n'
                << "  $ timew delete @2\n"
                << '\n'
                << "See also 'cancel'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "diagnostics")
      std::cout << '\n'
                << "Syntax: timew diagnostics\n"
                << '\n'
                << "This command shows details about your version of Timewarrior, your platform, how\n"
                << "it was built, compiler features, configuration, file access, extensions and more.\n"
                << '\n'
                << "The purpose of this command is to help diagnose configuration problems and\n"
                << "provide supplemental information when reporting a problem.\n"
                << '\n'
                << "See also 'extensions'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "dom")
      std::cout << '\n'
                << "Supported DOM references are:\n"
                << '\n'
                << "  dom.tag.count             Count of all tags\n"
                << "  dom.tag.1                 Nth tag used\n"
                << '\n'
                << "  dom.active                '1' if there is active tracking, otherwise '0'\n"
                << "  dom.active.tag.count      Count of active tags\n"
                << "  dom.active.tag.1          Active Nth tag\n"
                << "  dom.active.start          Active start timestamp (ISO Extended local date)\n"
                << "  dom.active.duration       Active elapsed (ISO Period)\n"
                << "  dom.active.json           Active interval as JSON\n"
                << '\n'
                << "  dom.tracked.count         Count of tracked intervals\n"
                << "  dom.tracked.1.tag.count   Count of active tags\n"
                << "  dom.tracked.1.tag.1       Tracked Nth, Nth tag\n"
                << "  dom.tracked.1.start       Tracked Nth, start time\n"
                << "  dom.tracked.1.end         Tracked Nth, end time, blank if closed\n"
                << "  dom.tracked.1.duration    Tracked Nth, elapsed\n"
                << "  dom.tracked.1.json        Tracked Nth, interval as JSON\n"
                << '\n'
                << "  dom.rc.<name>             Configuration setting\n"
                << '\n'
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "export")
      std::cout << '\n'
                << "Syntax: timew export [<interval>] [<tag> ...]\n"
                << '\n'
                << "Exports all the tracked time in JSON format. Supports filtering. For example:\n"
                << '\n'
                << "  $ timew export from 2016-01-01 for 3wks tag1\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "extensions")
      std::cout << '\n'
                << "Syntax: timew extensions\n"
                << '\n'
                << "Displays the directory containing the extension programs and a table showing\n"
                << "each extensions and its status.\n"
                << '\n'
                << "See also 'diagnostics'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
/*
    else if (words[0] == "fill")
      std::cout << '\n'
                << "Syntax: timew fill @<id> [@<id> ...]\n"
                << '\n'
                << "The 'fill' command is used to adjust any interval to fill in surrounding gaps.\n"
                << "Using the 'summary' command, and specifying the ':ids' hint shows interval IDs.\n"
                << "Using the right ID, you can identify an interval to fill. For example, show\n"
                << "the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to fill:\n"
                << '\n'
                << "  $ timew fill @2\n"
                << '\n'
                << "Note that you can fill multiple intervals:\n"
                << '\n'
                << "  $ timew fill @2 @10 @23\n"
                << '\n'
                << "See also 'hints'.\n"
                << '\n';
*/

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "gaps")
      std::cout << '\n'
                << "Syntax: timew gaps [<interval>] [<tag> ...]\n"
                << '\n'
                << "Displays a summary of time that is neither tracked nor excluded from tracking.\n"
                << '\n'
                << "The 'reports.gaps.range' configuration setting overrides the default date range.\n"
                << "The default date range shown is ':day'.\n"
                << '\n'
                << "The ':blank' hint causes only the excluded time to be shown, with no tracked\n"
                << "time.\n"
                << '\n'
                << "See also 'summary'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "get")
      std::cout << '\n'
                << "Syntax: timew get <DOM> [<DOM> ...]\n"
                << '\n'
                << "Validates the DOM reference, then obtains the value and displays it. For\n"
                << "example:\n"
                << '\n'
                << "  $ timew get dom.active\n"
                << "  1\n"
                << '\n'
                << "It is an error to reference an interval or tag that does not exist.\n"
                << '\n'
                << "See also 'dom'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "help")
      std::cout << '\n'
                << "Syntax: timew help [<command> | interval | hints | date | duration]\n"
                << "The help command shows detailed descriptions and examples of commands,\n"
                << "interval syntax, supported hints, date and duration formats and DOM references.\n"
                << "For example:\n"
                << '\n'
                << "  $ timew help\n"
                << "  $ timew help start\n"
                << "  $ timew help hints\n"
                << "  $ timew help interval\n"
                << "  $ timew help date\n"
                << "  $ timew help duration\n"
                << "  $ timew help dom\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "hints")
      std::cout << '\n'
                << "Timewarrior supports hints, which are single-word command line features that\n"
                << "start with a colon like this:\n"
                << '\n'
                << "  :week\n"
                << '\n'
                << "Hints serve several purposes. This example is a shortcut for the date range\n"
                << "that defines the current week. Other hints, such as:\n"
                << '\n'
                << "  :quiet\n"
                << '\n'
                << "are ways to control the behavior of Timewarrior, in this case eliminating all\n"
                << "forms of feedback, for purposes of automation. The supported hints are:\n"
                << '\n'
                << "  :quiet         Turns off all feedback. For automation\n"
                << "  :debug         Runs in debug mode, shows many runtime details\n"
                << "  :yes           Overrides confirmation by answering 'yes' to the questions\n"
                << '\n'
                << "  :color         Force color on, even if not connected to a TTY\n"
                << "  :nocolor       Force color off, even if connected to a TTY\n"
                << "  :blank         Leaves tracked time out of a report\n"
                << "  :fill          Expand time to fill surrounding available gap\n"
                << "  :adjust        Automatically correct overlaps\n"
                << "  :ids           Displays interval ID numbers in the summary report\n"
                << '\n'
                << "Range hints provide convenient shortcuts to date ranges:\n"
                << '\n'
                << "  :yesterday     The 24 hours of the previous day\n"
                << "  :day           The 24 hours of the current day\n"
                << "  :week          This week\n"
                << "  :month         This month\n"
                << "  :quarter       This quarter\n"
                << "  :year          This year\n"
                << "  :lastweek      Last week\n"
                << "  :lastmonth     Last month\n"
                << "  :lastquarter   Last quarter\n"
                << "  :lastyear      Last year\n"
                << '\n'
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "interval")
      std::cout << '\n'
                << "An interval defines a block of time that is tracked. The syntax for specifying\n"
                << "an interval is flexible, and may be one of:\n"
                << '\n'
                << "  [from] <date>\n"
                << "  [from] <date> to/- <date>\n"
                << "  [from] <date> for <duration>\n"
                << "  <duration> before/after <date>\n"
                << "  <duration> ago\n"
                << "  [for] <duration>\n"
                << '\n'
                << "Examples are:\n"
                << '\n'
                << "  from 9:00\n"
                << "  from 9am - 11am\n"
                << "  from 9:00:00 to 11:00\n"
                << "  from 9:00 for 2h\n"
                << "  2h after 9am\n"
                << "  2h before 11:00\n"
                << "  2h ago\n"
                << "  for 2h\n"
                << '\n'
                << "An interval is said to be 'closed' if there is both a start and end, and 'open'\n"
                << "if there is no end date.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "join")
      std::cout << '\n'
                << "Syntax: timew join @<id> @<id>\n"
                << '\n'
                << "Joins two intervals, by using the earlier of the two start times, and the later\n"
                << "of the two end times, and the combined set of tags. Using the 'summary' command,\n"
                << "and specifying the ':ids' hint shows interval IDs. Using the correct IDs, you\n"
                << "can identify an intervals to join. For example, show the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@1' and '@2' as the intervals you wish to join:\n"
                << '\n'
                << "  $ timew join @1 @2\n"
                << '\n'
                << "See also 'split', 'lengthen', 'shorten'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "lengthen")
      std::cout << '\n'
                << "Syntax: timew lengthen @<id> [@<id> ...] <duration>\n"
                << '\n'
                << "The 'lengthen' command is used to defer the end date of a closed interval.\n"
                << "Using the 'summary' command, and specifying the ':ids' hint shows interval IDs.\n"
                << "Using the right ID, you can identify an interval to lengthen. For example, show\n"
                << "the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to lengthen:\n"
                << '\n'
                << "  $ timew lengthen @2 10mins\n"
                << '\n'
                << "Note that you can lengthen multiple intervals:\n"
                << '\n'
                << "  $ timew lengthen @2 @10 @23 1hour\n"
                << '\n'
                << "See also 'summary', 'tag', 'untag', 'shorten'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "month")
      std::cout << '\n'
                << "Syntax: timew month [<interval>] [<tag> ...]\n"
                << '\n'
                << "The month command shows a chart depicting a single month (current month by\n"
                << "default), with colored blocks drawn on a timeline. The chart summarizes the\n"
                << "tracked and untracked time.\n"
                << '\n'
                << "Accepts date ranges and tags for filtering, or shortcut hints:\n"
                << '\n'
                << "  $ timew month 1st - today\n"
                << "  $ timew month :week\n"
                << '\n'
                << "The 'reports.month.range' configuration setting overrides the default date range.\n"
                << "The default date range shown is ':month'.\n"
                << '\n'
                << "The ':blank' hint causes only the excluded time to be shown, with no tracked\n"
                << "time.\n"
                << '\n'
                << "For more details, and precise times, use the 'summary' report.\n"
                << '\n'
                << "See also 'day', 'week', 'summary'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "move")
      std::cout << '\n'
                << "Syntax: timew move @<id> <date>\n"
                << '\n'
                << "The 'move' command is used to reposition an interval at a new start time.\n"
                << "Using the 'summary' command, and specifying the ':ids' hint shows interval IDs.\n"
                << "Using the right ID, you can identify an interval to move. For example, show\n"
                << "the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to move:\n"
                << '\n'
                << "  $ timew move @2 9am\n"
                << '\n'
                << "See also 'summary', 'tag', 'untag', 'lengthen', 'shorten'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "report")
      std::cout << '\n'
                << "Syntax: timew [report] <report> [<interval>] [<tag> ...]\n"
                << '\n'
                << "Runs an extension report, and supports filtering data.\n"
                << "The 'report' command itself is optional, which means that these two commands\n"
                << "are equivalent:\n"
                << '\n'
                << "  $ timew report foo :week\n"
                << "  $ timew        foo :week\n"
                << '\n'
                << "This does however assume there is a 'foo' extension installed.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "shorten")
      std::cout << '\n'
                << "Syntax: timew shorten @<id> [@<id> ...] <duration>\n"
                << '\n'
                << "The 'shorten' command is used to advance the end date of a closed interval.\n"
                << "Using the 'summary' command, and specifying the ':ids' hint shows interval IDs.\n"
                << "Using the right ID, you can identify an interval to shorten. For example, show\n"
                << "the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to shorten:\n"
                << '\n'
                << "  $ timew shorten @2 10mins\n"
                << '\n'
                << "Note that you can shorten multiple intervals:\n"
                << '\n'
                << "  $ timew shorten @2 @10 @23 1hour\n"
                << '\n'
                << "See also 'summary', 'tag', 'untag', 'lengthen'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "show")
      std::cout << '\n'
                << "Syntax: timew show\n"
                << '\n'
                << "Displays the effective configuration in hierarchical form.\n"
                << '\n'
                << "See also 'config'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "split")
      std::cout << '\n'
                << "Syntax: timew split @<id> [@<id> ...]\n"
                << '\n'
                << "Ѕplits an interval into two equally sized adjacent intervals, having the same\n"
                << "tags. Using the 'summary' command, and specifying the ':ids' hint shows interval\n"
                << " IDs. Using the right ID, you can identify an interval to split. For example,\n"
                << "show the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to split:\n"
                << '\n'
                << "  $ timew split @2\n"
                << '\n'
                << "See also 'join', 'lengthen', 'shorten'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "start")
      std::cout << '\n'
                << "Syntax: timew start [<date>] [<tag> ...]\n"
                << '\n'
                << "Begins tracking using the current time with any specified set of tags. If a tag\n"
                << "contains multiple words, therefore containing spaces, use quotes to surround the\n"
                << "whole tag. For example, this command specifies two tags ('weekend' and\n"
                << "'Home & Garden'), the second of which requires quotes.\n"
                << '\n'
                << "  $ timew start weekend 'Home & Garden'\n"
                << '\n'
                << "An optional date may be specified to indicate the intended start of the tracked\n"
                << "time:\n"
                << '\n'
                << "  $ time start 8am weekend 'Home & Garden'\n"
                << '\n'
                << "If there is a previous open interval, it will be closed at the given start time.\n"
                << '\n'
                << "Quotes are harmless if used unecessarily.\n"
                << '\n'
                << "See also 'continue', 'stop', 'track'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "stop")
      std::cout << '\n'
                << "Syntax: timew stop [<tag> ...]\n"
                << '\n'
                << "Stops tracking time.  If tags are specified, then they are no longer tracked.\n"
                << "If no tags are specified, all tracking stops. For example:\n"
                << '\n'
                << "  $ timew start tag1 tag2\n"
                << "  ...\n"
                << "  $ timew stop tag1\n"
                << '\n'
                << "Initially time is tracked for both 'tag1' and 'tag2', then 'tag1' tracking is\n"
                << "stopped, leaving tag2 active. To stop all tracking:\n"
                << '\n'
                << "  $ timew stop\n"
                << '\n'
                << "See also 'cancel', 'continue', 'start', 'track'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "summary")
      std::cout << '\n'
                << "Syntax: timew summary [<interval>] [<tag> ...]\n"
                << '\n'
                << "Displays a report summarizing tracked and untracked time for the current day by\n"
                << "default. Accepts date ranges and tags for filtering, or shortcut hints:\n"
                << '\n'
                << "  $ timew summary monday - today\n"
                << "  $ timew summary :week\n"
                << "  $ timew summary :month\n"
                << '\n'
                << "The ':ids' hint adds an 'ID' column to the summary report output for interval\n"
                << "modification.\n"
                << '\n'
                << "See also 'day', 'week', 'month', 'shorten', 'lengthen', 'tag', 'untag'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "tag")
      std::cout << '\n'
                << "Syntax: timew tag @<id> [@<id> ...] <tag> [<tag> ...]\n"
                << '\n'
                << "The 'tag' command is used to add a tag to an interval. Using the 'summary'\n"
                << "command, and specifying the ':ids' hint shows interval IDs. Using the right ID,\n"
                << "you can identify an interval to tag. For example, show the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to tag:\n"
                << '\n'
                << "  $ timew tag @2 'New Tag'\n"
                << '\n'
                << "Note that you can tag multiple intervals, with multiple tags:\n"
                << '\n'
                << "  $ timew tag @2 @10 @23 'Tag One' tag2 tag3\n"
                << '\n'
                << "See also 'summary', 'shorten', 'lengthen', 'untag'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "tags")
      std::cout << '\n'
                << "Syntax: timew tags\n"
                << '\n'
                << "Displays all the tags that have been used.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "track")
      std::cout << '\n'
                << "Syntax: timew track <interval> [<tag> ...]\n"
                << '\n'
                << "The track command is used to add tracked time in the past. Perhaps you forgot\n"
                << "to record time, or are just filling in old entries. For example:\n"
                << '\n'
                << "  $ timew track :yesterday 'Training Course'\n"
                << "  $ timew track 9am - 11am 'Staff Meeting'\n"
                << '\n'
                << "Note that the track command expects a closed interval (start and end time), when\n"
                << "recording. If a closed interval is not provided, the 'track' command behaves the\n"
                << "same as the 'start' command.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "untag")
      std::cout << '\n'
                << "Syntax: timew untag @<id> [@<id> ...] <tag> [<tag> ...]\n"
                << '\n'
                << "The 'untag' command is used to removed a tag from an interval. Using the\n"
                << "'summary' command, and specifying the ':ids' hint shows interval IDs. Using the\n"
                << "right ID, you can identify an interval to untag. For example, show the IDs:\n"
                << '\n'
                << "  $ timew summary :week :ids\n"
                << '\n'
                << "Then having selected '@2' as the interval you wish to tag:\n"
                << '\n'
                << "  $ timew untag @2 'Old Tag'\n"
                << '\n'
                << "Note that you can untag multiple intervals, with multiple tags:\n"
                << '\n'
                << "  $ timew untag @2 @10 @23 'Old Tag' tag2 tag3\n"
                << '\n'
                << "See also 'summary', 'shorten', 'lengthen', 'tag'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "week")
      std::cout << '\n'
                << "Syntax: timew week [<interval>] [<tag> ...]\n"
                << '\n'
                << "The week command shows a chart depicting a single week (current week by\n"
                << "default), with colored blocks drawn on a timeline. The chart summarizes the\n"
                << "tracked and untracked time.\n"
                << '\n'
                << "Accepts date ranges and tags for filtering, or shortcut hints:\n"
                << '\n'
                << "  $ timew week\n"
                << "  $ timew week monday - today\n"
                << '\n'
                << "The 'reports.week.range' configuration setting overrides the default date range.\n"
                << "The default date range shown is ':week'.\n"
                << '\n'
                << "The ':blank' hint causes only the excluded time to be shown, with no tracked\n"
                << "time.\n"
                << '\n'
                << "For more details, and precise times, use the 'summary' report.\n"
                << '\n'
                << "See also 'day', 'month', 'summary'.\n"
                << '\n';

    else
      std::cout << "No help available for '" << words[0] << "'\n";

    return 0;
  }

  return CmdHelpUsage (extensions);
}

////////////////////////////////////////////////////////////////////////////////
