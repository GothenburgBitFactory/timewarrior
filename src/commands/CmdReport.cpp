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
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
// Given a partial match for an extension script name, find the full patch of
// the extension it may match.
static std::string findExtension (
  Extensions& extensions,
  const std::string& partial)
{
  auto scripts = extensions.all ();
  std::vector <std::string> options;
  for (auto& script : scripts)
    options.push_back (File (script).name ());

  std::vector <std::string> matches;
  autoComplete (partial, options, matches, 3);

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
  const std::vector <std::string>& args,
  Rules& rules,
  Database& database,
  Extensions& extensions,
  Log& log)
{
  // TODO Identify report.
  if (args.size () > 2)
  {
    auto script = findExtension (extensions, args[2]);

    // TODO Default report?

    // Load all data.
    auto intervals = database.getAllIntervals ();

    // TODO Apply filter.

    // Compose Header info.
    std::stringstream header;
    //   TODO Configuration.
    //   TODO Exclusions.
    //   TODO Filter.
    //   TODO CLI.
    //   TODO Directory containing *.data files.
    header << "version=" << VERSION << "\n";

    // Compose JSON.
    std::stringstream json;
    json << "[\n";
    int counter = 0;
    for (auto& interval : intervals)
    {
      if (counter)
        json << ",\n";

      json << interval.json ();
      ++counter;
    }

    if (intervals.size ())
      json << "\n";

    json << "]\n";

    // Compose the input for the script.
    auto input = header.str ()
               + "\n"
               + json.str ();

    // Run the extensions.
    std::vector <std::string> output;
    extensions.callExtension (script, split (input, '\n'), output);

    // Display the output.
    for (auto& line : output)
      std::cout << line << "\n";
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
