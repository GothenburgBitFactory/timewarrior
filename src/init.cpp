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
#include <Database.h>
#include <Rules.h>
#include <Extensions.h>
#include <Log.h>
//#include <Grammar.h>
//#include <LR0.h>
#include <shared.h>
#include <format.h>
#include <FS.h>
#include <commands.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
bool lightweightVersionCheck (const std::vector <std::string>& args)
{
  if (args.size () == 2 && args[1] == "--version")
  {
    std::cout << VERSION << "\n";
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
void initializeDataAndRules (
  Database& database,
  Rules& rules,
  Log& log)
{
  // The $TIMEWARRIORDB environment variable overrides the default value of
  // ~/.timewarrior‥
  Directory dbLocation;
  char* override = getenv ("TIMEWARRIORDB");
  if (override)
  {
    log.write ("info", std::string ("TIMEWARRIORDB ") + override);
    dbLocation = Directory (override);
  }
  else
  {
    dbLocation = Directory ("~/.timewarrior");
    log.write ("info", std::string ("Using default DB location ") + dbLocation._data);
  }

  // If dbLocation does not exist, ask whether it should be created.
  bool shinyNewDatabase = false;
  if (! dbLocation.exists () &&
      confirm ("Create new database in " + dbLocation._data + "?"))
  {
    dbLocation.create (0700);
    log.write ("info", std::string ("Created missing database in ") + dbLocation._data);
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

  // TODO Load rule grammar.
/*
  File ruleFile ("./rule.grammar");
  Grammar ruleGrammar;
  ruleGrammar.debug (debug);
  ruleGrammar.loadFromFile (ruleFile);

  // Instantiate the parser.
  LR0 ruleParser;
  ruleParser.debug (debug);
  ruleParser.initialize (ruleGrammar);
*/

  // Load the configuration data.
  File configFile (dbLocation);
  configFile += "timewarrior.cfg";
  configFile.create (0600);
  rules.load (configFile._data);

  // This value is not written out to disk, as there would be no point. Having
  // located the config file, the 'db' location is already known. This is just
  // for subsequent internal use.
  rules.set ("db", dbLocation._data);
  log.write ("info", std::string ("  rc.db=") + rules.get ("db"));

  // Perhaps some subsequent code would like to know this is a new db and
  // possibly a first run.
  rules.set ("shiny", (shinyNewDatabase ? 1 : 0));

  // Initialize the database (no data read), but files are enumerated.
  database.initialize (data._data);
  log.write ("info", database.dump ());

  // TODO Give the log file a temp fake name.  To be removed.
  log.file (dbLocation._data + "/timewarrior.log");
  log.write ("info", rules.dump ());
}

////////////////////////////////////////////////////////////////////////////////
void initializeExtensions (
  Rules& rules,
  Extensions& extensions,
  Log& log)
{
  Directory extDir (rules.get ("db"));
  extDir += "extensions";

  extensions.initialize (extDir._data);

  log.write ("info", extensions.dump ());
}

////////////////////////////////////////////////////////////////////////////////
int dispatchCommand (
  const std::vector <std::string>& args,
  Database& database,
  Rules& rules,
  Extensions& extensions,
  Log& log)
{
  int status {0};

  // Log the command line.
  std::string combined;
  for (auto& arg : args)
  {
    if (arg.find (' ') != std::string::npos)
      combined += "'" + arg + "' ";
    else
      combined += arg + " ";
  }
  log.write ("command", combined);

  if (args.size () > 1)
  {
    std::vector <std::string> allCommands =
    {
      "help", "clear", "config", "continue", "define", "diagnostics", "export",
      "gaps", "import", "log", "report", "start", "stop", "tags", "track",
      "undo"
    };

    std::vector <std::string> matches;
    autoComplete (args[1], allCommands, matches);
    if (matches.size () == 1)
    {
      // These signatures are æxpected to be all different, therefore no
      // command to fn mapping.
           if (closeEnough (allCommands[0],  args[1], 2)) status = CmdHelp        ();
      else if (closeEnough (allCommands[1],  args[1], 2)) status = CmdClear       ();
      else if (closeEnough (allCommands[2],  args[1], 2)) status = CmdConfig      ();
      else if (closeEnough (allCommands[3],  args[1], 2)) status = CmdContinue    ();
      else if (closeEnough (allCommands[4],  args[1], 2)) status = CmdDefine      (rules);
      else if (closeEnough (allCommands[5],  args[1], 2)) status = CmdDiagnostics (database, log);
      else if (closeEnough (allCommands[6],  args[1], 2)) status = CmdExport      ();
      else if (closeEnough (allCommands[7],  args[1], 2)) status = CmdGaps        ();
      else if (closeEnough (allCommands[8],  args[1], 2)) status = CmdImport      ();
      else if (closeEnough (allCommands[9],  args[1], 2)) status = CmdLog         (args, log);
      else if (closeEnough (allCommands[10], args[1], 2)) status = CmdReport      ();
      else if (closeEnough (allCommands[11], args[1], 2)) status = CmdStart       ();
      else if (closeEnough (allCommands[12], args[1], 2)) status = CmdStop        ();
      else if (closeEnough (allCommands[13], args[1], 2)) status = CmdTags        ();
      else if (closeEnough (allCommands[14], args[1], 2)) status = CmdTrack       ();
      else if (closeEnough (allCommands[15], args[1], 2)) status = CmdUndo        ();
    }
    else if (matches.size () == 0)
    {
      CmdHelpUsage ();
    }
    else
    {
      std::cout << "timew: '" << args[1] << "' is not a command. See 'timew help'.\n"
                << "\n"
                << "Did you mean one of these?\n";

      for (const auto& command : matches)
        std::cout << "  " << command << "\n";
    }
  }
  else if (args.size () == 1)
  {
    status = CmdDefault ();
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////
