////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <commands.h>
#include <shared.h>
#include <format.h>
#include <timew.h>
#include <iostream>
#include <sstream>
#include <FS.h>
#include <IntervalFilterAllInRange.h>
#include <IntervalFilterAllWithTags.h>
#include <IntervalFilterAndGroup.h>

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
  {
    options.push_back (Path (script).name ());
  }

  std::vector <std::string> matches;
  autoComplete (partial, options, matches);

  if (matches.empty ())
  {
    throw format ("The report '{1}' is not recognized.", partial);
  }

  if (matches.size () > 1)
  {
    throw format ("The report '{1}' is ambiguous, and could mean one of: {2}",
                  partial,
                  join (", ", matches));
  }

  // Now map matches[0] back to it's corresponding option.
  for (auto& script : scripts)
  {
    if (Path (script).name () == matches[0])
    {
      return script;
    }
  }

  return "";
}

std::string basename (const std::string &script_path)
{
  const auto lastSlash = script_path.find_last_of ('/');

  if (lastSlash != std::string::npos)
  {
    return script_path.substr (lastSlash + 1);
  }

  return script_path;
}

std::string dropExtension (const std::string& basename)
{
  const auto lastDot = basename.find_last_of ('.');

  if (lastDot != std::string::npos)
  {
    return basename.substr (0, lastDot);
  }

  return basename;
}

std::string getScriptName (const std::string& script_path)
{
  return dropExtension (basename (script_path));
}

////////////////////////////////////////////////////////////////////////////////
int CmdReport (
  const CLI& cli,
  Rules& rules,
  Database& database,
  const Extensions& extensions)
{
  std::string script_path;
  for (auto& arg : cli._args)
  {
    if (arg.hasTag ("EXT"))
    {
      script_path = findExtension (extensions, arg.attribute ("canonical"));
    }
  }

  if (script_path.empty ())
  {
    throw std::string ("Specify which report to run.");
  }

  auto script_name = getScriptName (script_path);
  auto default_hint = rules.get ("reports.range", "all");
  auto report_hint = rules.get (format ("reports.{1}.range", script_name), default_hint);

  Range default_range = {};
  expandIntervalHint (":" + report_hint, default_range);

  // Create a filter, and if empty, choose the current week.
  auto filter = cli.getFilter (default_range);

  IntervalFilterAndGroup filtering ({
    std::make_shared <IntervalFilterAllInRange> ( Range { filter.start, filter.end }),
    std::make_shared <IntervalFilterAllWithTags> (filter.tags ())
  });

  auto tracked = getTracked (database, rules, filtering);

  // Compose Header info.
  rules.set ("temp.report.start", filter.is_started () ? filter.start.toISO () : "");
  rules.set ("temp.report.end",   filter.is_ended ()   ? filter.end.toISO ()   : "");
  rules.set ("temp.report.tags", joinQuotedIfNeeded (",", filter.tags ()));
  rules.set ("temp.version", VERSION);

  std::stringstream header;
  for (auto& name : rules.all ())
    header << name << ": " << rules.get (name) << '\n';

  // Get the data.
  auto input = header.str ()
             + '\n'
             + jsonFromIntervals (tracked);

  // Run the extensions.
  std::vector <std::string> output;
  int rc = extensions.callExtension (script_path, split (input, '\n'), output);
  if (rc != 0 && output.size () == 0)
  {
    throw format ("'{1}' returned {2} without producing output.", script_path, rc);
  }

  // Display the output.
  for (auto& line : output)
    std::cout << line << '\n';

  return rc;
}

////////////////////////////////////////////////////////////////////////////////
