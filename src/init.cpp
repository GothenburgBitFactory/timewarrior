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
#include <format.h>
#include <FS.h>
#include <commands.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
bool lightweightVersionCheck (int argc, const char** argv)
{
  if (argc == 2 && std::string (argv[1]) == "--version")
  {
    std::cout << VERSION << "\n";
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
void initializeEntities (CLI& cli)
{
  // Command entities.
  cli.entity ("command", "clear");
  cli.entity ("command", "config");
  cli.entity ("command", "continue");
  cli.entity ("command", "define");
  cli.entity ("command", "diagnostics");
  cli.entity ("command", "export");
  cli.entity ("command", "extensions");
  cli.entity ("command", "gaps");
  cli.entity ("command", "help");
  cli.entity ("command", "import");
  cli.entity ("command", "report");
  cli.entity ("command", "start");
  cli.entity ("command", "stop");
  cli.entity ("command", "tags");
  cli.entity ("command", "track");
  cli.entity ("command", "undo");

  // Hint entities.
  cli.entity ("hint", ":debug");
  cli.entity ("hint", ":quiet");
  cli.entity ("hint", ":week");
  cli.entity ("hint", ":month");
  cli.entity ("hint", ":quarter");
  cli.entity ("hint", ":year");
  cli.entity ("hint", ":fill");

  // TODO Extension names.
}

////////////////////////////////////////////////////////////////////////////////
void initializeDataAndRules (
  CLI& cli,
  Database& database,
  Rules& rules)
{
  // Make common hints available via rules:
  //   :debug --> debug
  //   :quiet --> verbose
  for (auto& arg : cli._args)
  {
    if (arg.hasTag ("HINT"))
    {
      if (arg.attribute ("canonical") == ":debug") rules.set ("debug", "on");
      if (arg.attribute ("canonical") == ":quiet") rules.set ("verbose", "off");
    }
  }

  if (rules.getBoolean ("debug"))
    std::cout << cli.dump () << "\n";

  // The $TIMEWARRIORDB environment variable overrides the default value of
  // ~/.timewarrior‥
  Directory dbLocation;
  char* override = getenv ("TIMEWARRIORDB");
  dbLocation = Directory (override ? override : "~/.timewarrior");

  // If dbLocation does not exist, ask whether it should be created.
  bool shinyNewDatabase = false;
  if (! dbLocation.exists () &&
      confirm ("Create new database in " + dbLocation._data + "?"))
  {
    dbLocation.create (0700);
    shinyNewDatabase = true;
  }

  // Create extensions subdirectory if necessary.
  Directory extensions (dbLocation);
  extensions += "extensions";
  if (! extensions.exists ())
    extensions.create (0700);

  // Create data subdirectory if necessary.
  Directory data (dbLocation);
  data += "data";
  if (! data.exists ())
    data.create (0700);

  // If dbLocation exists, but is not readable/writable/executable, error.
  if (dbLocation.exists () &&
      (! dbLocation.readable () ||
       ! dbLocation.writable () ||
       ! dbLocation.executable ()))
  {
    throw format ("Database is not readable at '{1}'", dbLocation._data);
  }

  // Load the configuration data.
  File configFile (dbLocation);
  configFile += "timewarrior.cfg";
  configFile.create (0600);
  rules.load (configFile._data);

  // TODO Provide the exclusions to the database, for use with new files.

  // This value is not written out to disk, as there would be no point. Having
  // located the config file, the 'db' location is already known. This is just
  // for subsequent internal use.
  rules.set ("temp.db", dbLocation._data);

  // Perhaps some subsequent code would like to know this is a new db and
  // possibly a first run.
  rules.set ("temp.shiny", (shinyNewDatabase ? 1 : 0));

  // Initialize the database (no data read), but files are enumerated.
  database.initialize (data._data);
}

////////////////////////////////////////////////////////////////////////////////
void initializeExtensions (
  Rules& rules,
  Extensions& extensions)
{
  Directory extDir (rules.get ("temp.db"));
  extDir += "extensions";

  extensions.initialize (extDir._data);
}

////////////////////////////////////////////////////////////////////////////////
int dispatchCommand (
  CLI& cli,
  Database& database,
  Rules& rules,
  Extensions& extensions)
{
  int status {0};

  // Dispatch to the right command function.
  std::string command = cli.getCommand ();
  if (command != "")
  {
    // These signatures are æxpected to be all different, therefore no
    // command to fn mapping.
         if (command == "clear")       status = CmdClear       (                                );
    else if (command == "config")      status = CmdConfig      (                                );
    else if (command == "continue")    status = CmdContinue    (     rules, database            );
    else if (command == "define")      status = CmdDefine      (cli, rules, database            );
    else if (command == "diagnostics") status = CmdDiagnostics (     rules, database, extensions);
    else if (command == "export")      status = CmdExport      (cli, rules, database            );
    else if (command == "extensions")  status = CmdExtensions  (     rules,           extensions);
    else if (command == "gaps")        status = CmdGaps        (                                );
    else if (command == "help")        status = CmdHelp        (cli                             );
    else if (command == "import")      status = CmdImport      (                                );
    else if (command == "report")      status = CmdReport      (cli, rules, database, extensions);
    else if (command == "start")       status = CmdStart       (cli, rules, database            );
    else if (command == "stop")        status = CmdStop        (cli, rules, database            );
    else if (command == "tags")        status = CmdTags        (     rules, database            );
    else if (command == "track")       status = CmdTrack       (cli, rules, database            );
    else if (command == "undo")        status = CmdUndo        (                                );
  }
  else
  {
    status = CmdDefault (rules, database);
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////
