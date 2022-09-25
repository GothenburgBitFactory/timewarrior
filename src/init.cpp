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
#include <timew.h>
#include <shared.h>
#include <format.h>
#include <commands.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <paths.h>

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
  cli.entity ("command", "annotate");
  cli.entity ("command", "cancel");
  cli.entity ("command", "config");
  cli.entity ("command", "continue");
  cli.entity ("command", "delete");
  cli.entity ("command", "diagnostics");
  cli.entity ("command", "export");
  cli.entity ("command", "extensions");
  cli.entity ("command", "fill");
  cli.entity ("command", "gaps");
  cli.entity ("command", "get");
  cli.entity ("command", "help");
  cli.entity ("command", "--help");
  cli.entity ("command", "-h");
  cli.entity ("command", "join");
  cli.entity ("command", "lengthen");
  cli.entity ("command", "modify");
  cli.entity ("command", "move");
  cli.entity ("command", "report");
  cli.entity ("command", "resize");
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

  // Some command list themselves as extensions, to integrate with the real extensions.
  cli.entity ("extension", "day");
  cli.entity ("extension", "month");
  cli.entity ("extension", "summary");
  cli.entity ("extension", "week");

  // Hint entities.
  cli.entity ("hint", ":all");
  cli.entity ("hint", ":adjust");
  cli.entity ("hint", ":blank");
  cli.entity ("hint", ":color");
  cli.entity ("hint", ":day");
  cli.entity ("hint", ":debug");
  cli.entity ("hint", ":fill");
  cli.entity ("hint", ":ids");
  cli.entity ("hint", ":no-ids");
  cli.entity ("hint", ":tags");
  cli.entity ("hint", ":no-tags");
  cli.entity ("hint", ":annotations");
  cli.entity ("hint", ":no-annotations");
  cli.entity ("hint", ":holidays");
  cli.entity ("hint", ":no-holidays");
  cli.entity ("hint", ":lastmonth");
  cli.entity ("hint", ":lastquarter");
  cli.entity ("hint", ":lastweek");
  cli.entity ("hint", ":lastyear");
  cli.entity ("hint", ":month");
  cli.entity ("hint", ":nocolor");
  cli.entity ("hint", ":quarter");
  cli.entity ("hint", ":quiet");
  cli.entity ("hint", ":week");
  cli.entity ("hint", ":fortnight");
  cli.entity ("hint", ":year");
  cli.entity ("hint", ":yes");
  cli.entity ("hint", ":yesterday");
  cli.entity ("hint", ":monday");
  cli.entity ("hint", ":tuesday");
  cli.entity ("hint", ":wednesday");
  cli.entity ("hint", ":thursday");
  cli.entity ("hint", ":friday");
  cli.entity ("hint", ":saturday");
  cli.entity ("hint", ":sunday");
}

////////////////////////////////////////////////////////////////////////////////
void initializeDataJournalAndRules (
  const CLI& cli,
  Database& database,
  Journal& journal,
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
  paths::initializeDirs (cli, rules);

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

  std::string dbDataDir = paths::dbDataDir ();
  journal.initialize (dbDataDir + "/undo.data", rules.getInteger ("journal.size"));
  // Initialize the database (no data read), but files are enumerated.
  database.initialize (dbDataDir, journal);
}

////////////////////////////////////////////////////////////////////////////////
void initializeExtensions (
  CLI& cli,
  const Rules& rules,
  Extensions& extensions)
{
  Directory extDir (paths::extensionsDir ());

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
  Journal& journal,
  Rules& rules,
  const Extensions& extensions)
{
  int status {0};

  // Debug output.
  if (rules.getBoolean ("debug"))
    std::cout << cli.dump () << '\n';

  // Dispatch to the right command function.
  std::string command = cli.getCommand ();

  if (! command.empty ())
  {
    // These signatures are expected to be all different, therefore no command to fn mapping.
         if (command == "annotate")    status = CmdAnnotate      (cli, rules, database, journal            );
    else if (command == "cancel")      status = CmdCancel        (     rules, database, journal            );
    else if (command == "config")      status = CmdConfig        (cli, rules,           journal            );
    else if (command == "continue")    status = CmdContinue      (cli, rules, database, journal            );
    else if (command == "day")         status = CmdChartDay      (cli, rules, database                     );
    else if (command == "delete")      status = CmdDelete        (cli, rules, database, journal            );
    else if (command == "diagnostics") status = CmdDiagnostics   (     rules, database,          extensions);
    else if (command == "export")      status = CmdExport        (cli, rules, database                     );
    else if (command == "extensions")  status = CmdExtensions    (                               extensions);
/*
    else if (command == "fill")        status = CmdFill          (cli, rules, database, journal            );
//*/
    else if (command == "gaps")        status = CmdGaps          (cli, rules, database                     );
    else if (command == "get")         status = CmdGet           (cli, rules, database                     );
    else if (command == "help" ||
             command == "--help" ||
             command == "-h")          status = CmdHelp          (cli,                           extensions);
    else if (command == "join")        status = CmdJoin          (cli, rules, database, journal            );
    else if (command == "lengthen")    status = CmdLengthen      (cli, rules, database, journal            );
    else if (command == "modify")      status = CmdModify        (cli, rules, database, journal            );
    else if (command == "month")       status = CmdChartMonth    (cli, rules, database                     );
    else if (command == "move")        status = CmdMove          (cli, rules, database, journal            );
    else if (command == "report")      status = CmdReport        (cli, rules, database,          extensions);
    else if (command == "resize")      status = CmdResize        (cli, rules, database, journal            );
    else if (command == "shorten")     status = CmdShorten       (cli, rules, database, journal            );
    else if (command == "show")        status = CmdShow          (     rules                               );
    else if (command == "split")       status = CmdSplit         (cli, rules, database, journal            );
    else if (command == "start")       status = CmdStart         (cli, rules, database, journal            );
    else if (command == "stop")        status = CmdStop          (cli, rules, database, journal            );
    else if (command == "summary")     status = CmdSummary       (cli, rules, database                     );
    else if (command == "tag")         status = CmdTag           (cli, rules, database, journal            );
    else if (command == "tags")        status = CmdTags          (cli, rules, database                     );
    else if (command == "track")       status = CmdTrack         (cli, rules, database, journal            );
    else if (command == "undo")        status = CmdUndo          (     rules, database, journal            );
    else if (command == "untag")       status = CmdUntag         (cli, rules, database, journal            );
    else if (command == "week")        status = CmdChartWeek     (cli, rules, database                     );
    else                               status = CmdReport        (cli, rules, database,          extensions);
  }
  else
  {
    auto words = cli.getWords ();

    if (! words.empty ())
    {
      throw format ("'{1}' is not a timew command. See 'timew help'.", words[0]);
    }
    else
    {
      status = CmdDefault (rules, database);
    }
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////
