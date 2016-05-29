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
#include <commands.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdHelpUsage ()
{
  // TODO This is going to need more formatting.
  std::cout << '\n'
            << "Usage: timew [--version]\n"
            << "       timew cancel\n"
            << "       timew config [<name> [<value | '']]\n"
            << "       timew continue\n"
            << "       timew day [<interval>] [<tag> ...]\n"
            << "       timew diagnostics\n"
            << "       timew export [<interval>] [<tag> ...]\n"
            << "       timew extensions\n"
            << "       timew gaps [<interval>] [<tag> ...]\n"
            << "       timew help [<command> | hints]\n"
            << "       timew month [<interval>] [<tag> ...]\n"
            << "       timew [report] <report> [<interval>] [<tag> ...]\n"
            << "       timew show\n"
            << "       timew start [<date>] [<tag> ...]\n"
            << "       timew stop [<tag> ...]\n"
            << "       timew summary [<interval>] [<tag> ...]\n"
            << "       timew tags\n"
            << "       timew track <interval> [<tag> ...]\n"
            << "       timew week [<interval>] [<tag> ...]\n"
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

  // TODO clear
  // TODO import
  // TODO quarter
  // TODO undo
  // TODO year

  // TODO List all extensions.

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TODO Perhaps have a set of 'documents' to display, and a keyword index that
//      maps to the the appropriate ones. Then display a list of any ambiguous
//      requests, or the appropriate doc for single hits. Perhaps a 'See also'.
//
// TODO Should the CmdXxx functions themselves be responsible for providing both
//      the command syntax and the help text?
//
// Strict 80-character limit.
// Provide examples where appropriate - enough to cover all uses.
int CmdHelp (const CLI& cli)
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
                << "If there is an open interval, close and abandon it.\n"
                << '\n'
                << "See also 'start', 'stop'.\n"
                << '\n';

    // TODO clear

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
                << "sought. To override this confirmation, use the ':yes' hint:\n"
                << '\n'
                << "  $ timew config verbose yes\n"
                << "  $ timew config verbose ''\n"
                << "  $ timew config verbose :yes\n"
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
                << "  $ timew track 9am - 5pm tag1 tag2\n"
                << "  $ timew continue\n"
                << '\n'
                << "The 'continue' command creates a new interval, starting now, and using the tags\n"
                << "'tag1' and 'tag2'.\n"
                << '\n'
                << "See also 'start', 'stop'.\n"
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
                << "time.\n"
                << '\n'
                << "For more details, and precise times, use the 'summary' report.\n"
                << '\n'
                << "See also 'week', 'month', 'summary'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "diagnostics")
      std::cout << '\n'
                << "Syntax: timew diagnostics\n"
                << '\n'
                << "This command shows details about your version of Timewarrior, platform, how it\n"
                << "was built, compiler features, configuration, file access, extensions and more.\n"
                << '\n'
                << "The purpose of this command is to help diagnose configuration problems, and\n"
                << "provide supplemental information when reporting a problem.\n"
                << '\n'
                << "See also 'extensions'.\n"
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
                << '\n'
                << "See also 'import'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "extensions")
      std::cout << '\n'
                << "Syntax: timew extensions\n"
                << '\n'
                << "Displays the directory containing the extension programs, and a table showing\n"
                << "each extensions and its status.\n"
                << '\n'
                << "See also 'diagnostics'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "gaps")
      std::cout << '\n'
                << "Syntax: timew gaps [<interval>] [<tag> ...]\n"
                << '\n'
                << "Displays a summary of time that is neither tracked, nor excluded from tracking.\n"
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
    else if (words[0] == "help")
      std::cout << '\n'
                << "The help command shows detailed descriptions and examples of commands and the\n"
                << "supported hints. For example:\n"
                << '\n'
                << "  $ timew help\n"
                << "  $ timew help start\n"
                << "  $ timew help hints\n"
                << "  $ timew help interval\n"
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
                << "                 Only functions when exclusions are provided\n"
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

    // TODO import
    // TODO quarter
    // TODO report

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "show")
      std::cout << '\n'
                << "Syntax: timew show\n"
                << '\n'
                << "Displays the effective configuration, in hierarchical form.\n"
                << '\n'
                << "See also 'config'.\n"
                << '\n';

    // Ruler                 1         2         3         4         5         6         7         8
    //              12345678901234567890123456789012345678901234567890123456789012345678901234567890
    else if (words[0] == "start")
      std::cout << '\n'
                << "Syntax: timew start [<date>] [<tag> ...]\n"
                << '\n'
                << "Begins tracking using the current time, and the specified optional set of tags.\n"
                << "If a tag contains multiple words, and therefore contains spaces, use quotes to\n"
                << "surround the whole tag. For example, this command specifies two tags ('weekend'\n"
                << "and 'Home & Garden'), the second of which requires quotes.\n"
                << '\n'
                << "  $ timew start weekend 'Home & Garden'\n"
                << '\n'
                << "An optional date may be specified to indicate the intended start to the tracked\n"
                << "time:\n"
                << '\n'
                << "  $ time start 8am weekend 'Home & Garden'\n"
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
                << "Displays a report summarizing tracked and untracked time for\n"
                << "the current day by default. Accepts date ranges and tags for\n"
                << "filtering, or shortcut hints:\n"
                << '\n'
                << "  $ timew summary monday - today\n"
                << "  $ timew summary :week\n"
                << "  $ timew summary :month\n"
                << '\n'
                << "See also 'day', 'week', 'month'.\n"
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

    // TODO undo
    // TODO year

    else
      std::cout << "No help available for '" << words[0] << "'\n";

    return 0;
  }

  return CmdHelpUsage ();
}

////////////////////////////////////////////////////////////////////////////////
