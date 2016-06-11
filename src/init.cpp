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
#include <timew.h>
#include <CLI.h>
#include <Database.h>
#include <Rules.h>
#include <Extensions.h>
#include <Datetime.h>
#include <shared.h>
#include <format.h>
#include <FS.h>
#include <commands.h>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
bool lightweightVersionCheck (int argc, const char** argv)
{
  if (argc == 2 && std::string (argv[1]) == "--version")
  {
    std::cout << VERSION << '\n';
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
void initializeEntities (CLI& cli)
{
  // Command entities.
  cli.entity ("command", "cancel");
  cli.entity ("command", "clear");
  cli.entity ("command", "config");
  cli.entity ("command", "continue");
  cli.entity ("command", "diagnostics");
  cli.entity ("command", "export");
  cli.entity ("command", "extensions");
  cli.entity ("command", "gaps");
  cli.entity ("command", "help");
  cli.entity ("command", "import");
  cli.entity ("command", "join");
  cli.entity ("command", "lengthen");
  cli.entity ("command", "move");
  cli.entity ("command", "report");
  cli.entity ("command", "shorten");
  cli.entity ("command", "show");
  cli.entity ("command", "split");
  cli.entity ("command", "start");
  cli.entity ("command", "stop");
  cli.entity ("command", "tag");
  cli.entity ("command", "tags");
  cli.entity ("command", "track");
  cli.entity ("command", "undo");
  cli.entity ("command", "untag");

  // Some command list themselves as extensions, to integrate with the real
  // extensions.
  cli.entity ("extension", "day");
  cli.entity ("extension", "month");
  cli.entity ("extension", "summary");
  cli.entity ("extension", "week");

  // Hint entities.
  cli.entity ("hint", ":adjust");
  cli.entity ("hint", ":blank");
  cli.entity ("hint", ":color");
  cli.entity ("hint", ":day");
  cli.entity ("hint", ":debug");
  cli.entity ("hint", ":fill");
  cli.entity ("hint", ":ids");
  cli.entity ("hint", ":lastmonth");
  cli.entity ("hint", ":lastquarter");
  cli.entity ("hint", ":lastweek");
  cli.entity ("hint", ":lastyear");
  cli.entity ("hint", ":month");
  cli.entity ("hint", ":nocolor");
  cli.entity ("hint", ":quarter");
  cli.entity ("hint", ":quiet");
  cli.entity ("hint", ":week");
  cli.entity ("hint", ":year");
  cli.entity ("hint", ":yes");
  cli.entity ("hint", ":yesterday");
}

////////////////////////////////////////////////////////////////////////////////
void initializeDataAndRules (
  const CLI& cli,
  Database& database,
  Rules& rules)
{
  // Rose tint my world, make me safe from my trouble and pain.
  rules.set ("color", isatty (STDOUT_FILENO) ? "on" : "off");

  // Make common hints available via rules:
  //   :debug   --> debug=on
  //   :quiet   --> verbose=off
  //   :color   --> color=on
  //   :nocolor --> color=off
  //   :yes     --> confirmation=off
  for (auto& arg : cli._args)
  {
    if (arg.hasTag ("HINT"))
    {
      if (arg.attribute ("canonical") == ":debug")   rules.set ("debug",        "on");
      if (arg.attribute ("canonical") == ":quiet")   rules.set ("verbose",      "off");
      if (arg.attribute ("canonical") == ":color")   rules.set ("color",        "on");
      if (arg.attribute ("canonical") == ":nocolor") rules.set ("color",        "off");
      if (arg.attribute ("canonical") == ":yes")     rules.set ("confirmation", "off");
    }
  }

  enableDebugMode (rules.getBoolean ("debug"));

  // The $TIMEWARRIORDB environment variable overrides the default value of
  // ~/.timewarrior‥
  Directory dbLocation;
  char* override = getenv ("TIMEWARRIORDB");
  dbLocation = Directory (override ? override : "~/.timewarrior");

  // If dbLocation exists, but is not readable/writable/executable, error.
  if (dbLocation.exists () &&
      (! dbLocation.readable () ||
       ! dbLocation.writable () ||
       ! dbLocation.executable ()))
  {
    throw format ("Database is not readable at '{1}'", dbLocation._data);
  }

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

  // Load the configuration data.
  File configFile (dbLocation);
  configFile += "timewarrior.cfg";
  configFile.create (0600);
  rules.load (configFile._data);

  // This value is not written out to disk, as there would be no point. Having
  // located the config file, the 'db' location is already known. This is just
  // for subsequent internal use.
  rules.set ("temp.db", dbLocation._data);

  // Perhaps some subsequent code would like to know this is a new db and
  // possibly a first run.
  if (shinyNewDatabase)
    rules.set ("temp.shiny", 1);

  if (rules.has ("debug.indicator"))
    setDebugIndicator (rules.get ("debug.indicator"));

  if (rules.has ("theme.colors.debug"))
    setDebugColor (Color (rules.get ("theme.colors.debug")));

  // Apply command line overrides.
  for (auto& arg : cli._args)
  {
    if (arg.hasTag ("CONFIG"))
    {
      rules.set (arg.attribute ("name"), arg.attribute ("value"));
      debug (format ("Configuration override {1} = {2}", arg.attribute ("name"), arg.attribute ("value")));
    }
  }

  // Initialize the database (no data read), but files are enumerated.
  database.initialize (data._data);

  // Set date names like "monday" to represent the past, not the future.
  Datetime::lookForwards = false;
}

////////////////////////////////////////////////////////////////////////////////
void initializeExtensions (
  CLI& cli,
  const Rules& rules,
  Extensions& extensions)
{
  Directory extDir (rules.get ("temp.db"));
  extDir += "extensions";

  extensions.initialize (extDir._data);

  // Add extensions as CLI entities.
  for (auto& ext : extensions.all ())
    cli.entity ("extension", File (ext).name ());

  // Extensions have a debug mode.
  if (rules.getBoolean ("debug"))
    extensions.debug ();
}

////////////////////////////////////////////////////////////////////////////////
int dispatchCommand (
  const CLI& cli,
  Database& database,
  Rules& rules,
  const Extensions& extensions)
{
  int status {0};

  // Debug output.
  if (rules.getBoolean ("debug"))
    std::cout << cli.dump () << '\n';

  // Dispatch to the right command function.
  std::string command = cli.getCommand ();
  if (command != "")
  {
    // These signatures are æxpected to be all different, therefore no
    // command to fn mapping.
         if (command == "cancel")      status = CmdCancel        (     rules, database            );
    else if (command == "clear")       status = CmdClear         (cli, rules, database            );
    else if (command == "config")      status = CmdConfig        (cli, rules, database            );
    else if (command == "continue")    status = CmdContinue      (     rules, database            );
    else if (command == "day")         status = CmdChartDay      (cli, rules, database            );
    else if (command == "diagnostics") status = CmdDiagnostics   (     rules, database, extensions);
    else if (command == "export")      status = CmdExport        (cli, rules, database            );
    else if (command == "extensions")  status = CmdExtensions    (     rules,           extensions);
    else if (command == "gaps")        status = CmdGaps          (cli, rules, database            );
    else if (command == "help")        status = CmdHelp          (cli                             );
    else if (command == "import")      status = CmdImport        (                                );
    else if (command == "join")        status = CmdJoin          (cli, rules, database            );
    else if (command == "lengthen")    status = CmdLengthen      (cli, rules, database            );
    else if (command == "month")       status = CmdChartMonth    (cli, rules, database            );
    else if (command == "move")        status = CmdMove          (cli, rules, database            );
    else if (command == "report")      status = CmdReport        (cli, rules, database, extensions);
    else if (command == "shorten")     status = CmdShorten       (cli, rules, database            );
    else if (command == "show")        status = CmdShow          (     rules                      );
    else if (command == "split")       status = CmdSplit         (cli, rules, database            );
    else if (command == "start")       status = CmdStart         (cli, rules, database            );
    else if (command == "stop")        status = CmdStop          (cli, rules, database            );
    else if (command == "summary")     status = CmdSummary       (cli, rules, database            );
    else if (command == "tag")         status = CmdTag           (cli, rules, database            );
    else if (command == "tags")        status = CmdTags          (     rules, database            );
    else if (command == "track")       status = CmdTrack         (cli, rules, database            );
    else if (command == "undo")        status = CmdUndo          (                                );
    else if (command == "untag")       status = CmdUntag         (cli, rules, database            );
    else if (command == "week")        status = CmdChartWeek     (cli, rules, database            );
    else                               status = CmdReport        (cli, rules, database, extensions);
  }
  else
  {
    status = CmdDefault (rules, database);
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////
