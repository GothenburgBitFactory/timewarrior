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
#include <shared.h>
#include <format.h>
#include <timew.h>
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
// Given a partial match for an extension script name, find the full patch of
// the extension it may match.
static std::string findExtension (
  const Extensions& extensions,
  const std::string& partial)
{
  auto scripts = extensions.all ();
  std::vector <std::string> options;
  for (auto& script : scripts)
    options.push_back (File (script).name ());

  std::vector <std::string> matches;
  autoComplete (partial, options, matches);

  if (matches.size () == 0)
    throw format ("The report '{1}' is not recognized.", partial);

  if (matches.size () > 1)
    throw format ("The report '{1}' is ambiguous, and could mean one of: {2}",
                  partial,
                  join (", ", matches));

  // Now map matches[0] back to it's corresponding option.
  for (auto& script : scripts)
    if (File (script).name () == matches[0])
      return script;

  return "";
}

////////////////////////////////////////////////////////////////////////////////
int CmdReport (
  const CLI& cli,
  Rules& rules,
  Database& database,
  const Extensions& extensions)
{
  std::string script;
  for (auto& arg : cli._args)
    if (arg.hasTag ("EXT"))
      script = findExtension (extensions, arg.attribute ("canonical"));

  if (script == "")
    throw std::string ("Specify which report to run.");

  // Filter the data.
  auto filter = createFilterIntervalFromCLI (cli);
  auto timeline = createTimelineFromData (rules, database, filter);
  auto intervals = timeline.tracked (rules);

  // Compose Header info.
  rules.set ("temp.report.start", filter.range.start.toEpoch () > 0 ? filter.range.start.toISO () : "");
  rules.set ("temp.report.end",   filter.range.end.toEpoch ()   > 0 ? filter.range.end.toISO ()   : "");
  std::string combinedTags;
  for (auto& tag : filter.tags ())
  {
    if (combinedTags != "")
      combinedTags += ",";
    combinedTags += quoteIfNeeded (tag);
  }
  rules.set ("temp.report.tags", combinedTags);

  std::stringstream header;
  for (auto& name : rules.all ())
    header << name << ": " << rules.get (name) << '\n';

  auto input = header.str ()
             + '\n'
             + jsonFromIntervals (intervals);

  // Run the extensions.
  std::vector <std::string> output;
  extensions.callExtension (script, split (input, '\n'), output);

  // Display the output.
  for (auto& line : output)
    std::cout << line << '\n';

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
