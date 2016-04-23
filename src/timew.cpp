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
#include <CLI.h>
#include <Database.h>
#include <Rules.h>
#include <Extensions.h>
#include <shared.h>
#include <commands.h>
#include <timew.h>
#include <iostream>
#include <new>

////////////////////////////////////////////////////////////////////////////////
int main (int argc, const char** argv)
{
  // Lightweight version checking that doesn't require initialization or I/O.
  int status = 0;
  if (lightweightVersionCheck (argc, argv))
    return status;

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

  try
  {
    // Scan command line.
    cli.analyze ();

    // Prepare the database, but do not read data.
    Database database;
    Rules rules;
    initializeDataAndRules (cli, database, rules);

    // Load extension script info.
    // Re-analyze command because of the new extention entities.
    Extensions extensions;
    initializeExtensions (cli, rules, extensions);
    cli.analyze ();

    // Dispatch to commands.
    status = dispatchCommand (cli, database, rules, extensions);

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

  return status;
}

////////////////////////////////////////////////////////////////////////////////
