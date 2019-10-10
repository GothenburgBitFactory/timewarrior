////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <CLI.h>
#include <Database.h>
#include <Rules.h>
#include <Extensions.h>
#include <Datetime.h>
#include <Timer.h>
#include <shared.h>
#include <commands.h>
#include <timew.h>
#include <iostream>
#include <iomanip>
#include <new>

////////////////////////////////////////////////////////////////////////////////
int main (int argc, const char** argv)
{
  Timer run_time;

  // Lightweight version checking that doesn't require initialization or I/O.
  int status = 0;
  if (lightweightVersionCheck (argc, argv))
    return status;

  try
  {
    // Timewarrior has special handling needs for times, such that a time that
    // is before the current time is not projected forwards to tomorrow. For
    // example:
    //
    //   now:    2017-05-21T12:22:00
    //   input:                10:00
    //   result: 2017-05-21T10:00:00
    Datetime::timeRelative = false;

    // Add entities so that command line tokens such as 'help' are recognized as
    // commands.
    CLI cli;
    initializeEntities (cli);

    // Capture the args.
    std::string commandLine;
    for (int i = 0; i < argc; i++)
    {
      cli.add (argv[i]);

      if (i)
        commandLine += " ";

      commandLine += quoteIfNeeded (argv[i]);
    }

    // Scan command line.
    cli.analyze ();

    Journal journal;
    // Prepare the database, but do not read data.
    Database database;
    Rules rules;
    initializeDataJournalAndRules (cli, database, journal, rules);

    // Load extension script info.
    // Re-analyze command because of the new extension entities.
    Extensions extensions;
    initializeExtensions (cli, rules, extensions);
    cli.analyze ();

    // Dispatch to commands.
    status = dispatchCommand (cli, database, journal, rules, extensions);

    // Save any outstanding changes.
    database.commit ();
  }

  catch (const std::string& error)
  {
    std::cerr << error << '\n';
    status = -1;
  }

  catch (std::bad_alloc& error)
  {
    auto message = std::string ("Memory allocation failed: ") + error.what ();
    std::cerr << "Error: " << message << '\n';
    status = -3;
  }

  catch (...)
  {
    auto message = "Unknown problem, please report.";
    std::cerr << "Error: " << message << '\n';
    status = -2;
  }

  run_time.stop ();
  std::stringstream s;
  s << "Timer timew "
    << std::setprecision (6)
    << std::fixed
    << run_time.total_us () / 1000000.0
    << " sec\n";
  debug (s.str ());

  return status;
}

////////////////////////////////////////////////////////////////////////////////
