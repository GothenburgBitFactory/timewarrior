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
#include <Configuration.h>
#include <Database.h>
#include <Rules.h>
#include <Extensions.h>
//#include <Grammar.h>
//#include <LR0.h>
#include <common.h>
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
  if (argc == 2 && ! std::strcmp (argv[1], "--version"))
  {
    std::cout << VERSION << "\n";
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
void initializeData (Configuration& configuration, Database& database)
{
  // The $TIMEWARRIORDB environment variable overrides the default value of
  // ~/.timewarrior‥
  Directory dbLocation;
  char* override = getenv ("TIMEWARRIORDB");
  if (override)
  {
    std::cout << "# TIMEWARRIORDB " << override << "\n";
    dbLocation = Directory (override);
  }
  else
  {
    dbLocation = Directory ("~/.timewarrior");
    std::cout << "# Using default DB location " << dbLocation._data << "\n";
  }

  // If dbLocation does not exist, ask whether it should be created.
  bool shinyNewDatabase = false;
  if (! dbLocation.exists () &&
      confirm ("Create new database in " + dbLocation._data + "?"))
  {
    dbLocation.create (0700);
    std::cout << "# Created missing database in " << dbLocation._data << "\n";
    shinyNewDatabase = true;
  }

  // Create extensions subdirectory if necessary.
  Directory extensions (dbLocation);
  extensions += "extensions";
  if (! extensions.exists ())
    extensions.create (0700);

  // Create extensions subdirectory if necessary.
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
  configuration.load (configFile._data);

  // This value is not written out to disk, as there would be no point. Having
  // located the config file, the 'db' location is already known. This is just
  // for subsequent internal use.
  configuration.set ("db", dbLocation._data);
  std::cout << "#   rc.db=" << configuration.get ("db") << "\n";

  // Perhaps some subsequent code would like to know this is a new db and
  // possibly a first run.
  configuration.set ("shiny", (shinyNewDatabase ? 1 : 0));

  // Initialize the database (no data read), but files are enumerated.
  database.initialize (data._data);

  std::cout << "# Configuration\n";
  for (const auto& name : configuration.all ())
    std::cout << "#   " << name << "=" << configuration[name] << "\n";

  std::cout << database.dump ();
}

////////////////////////////////////////////////////////////////////////////////
void initializeRules (Configuration& configuration, Rules& rules)
{
  // TODO Load rule grammar.
/*
  File ruleFile ("./rule.grammar");
  Grammar ruleGrammar;
  ruleGrammar.debug (debug);
  ruleGrammar.loadFromFile (ruleFile);
*/
  // Instantiate the parser.
/*
  LR0 ruleParser;
  ruleParser.debug (debug);
  ruleParser.initialize (ruleGrammar);
*/

  std::cout << "# " << rules.dump ();
}

////////////////////////////////////////////////////////////////////////////////
void initializeExtensions (Configuration& configuration, Extensions& extensions)
{
  Directory extDir (configuration.get ("db"));
  extDir += "extensions";

  extensions.initialize (extDir._data);

  std::cout << "# " << extensions.dump ();
}

////////////////////////////////////////////////////////////////////////////////
int dispatchCommand (
  int argc,
  const char** argv,
  Configuration& configuration,
  Database& database,
  Rules& rules,
  Extensions& extensions)
{
  int status {0};

  if (argc > 1)
  {
    std::vector <std::string> allCommands =
    {
      "help", "clear", "config", "continue", "define", "export", "gaps",
      "import", "log", "report", "start", "stop", "tags", "track"
    };

    std::vector <std::string> matches;
    autoComplete (argv[1], allCommands, matches);
    if (matches.size () == 1)
    {
      // These signatures are æxpected to be all different, therefore no
      // command to fn mapping.
           if (closeEnough (allCommands[0],  argv[1], 2)) status = CmdHelp     ();
      else if (closeEnough (allCommands[1],  argv[1], 2)) status = CmdClear    ();
      else if (closeEnough (allCommands[2],  argv[1], 2)) status = CmdConfig   ();
      else if (closeEnough (allCommands[3],  argv[1], 2)) status = CmdContinue ();
      else if (closeEnough (allCommands[4],  argv[1], 2)) status = CmdDefine   (rules);
      else if (closeEnough (allCommands[5],  argv[1], 2)) status = CmdExport   ();
      else if (closeEnough (allCommands[6],  argv[1], 2)) status = CmdGaps     ();
      else if (closeEnough (allCommands[7],  argv[1], 2)) status = CmdImport   ();
      else if (closeEnough (allCommands[8],  argv[1], 2)) status = CmdLog      ();
      else if (closeEnough (allCommands[9],  argv[1], 2)) status = CmdReport   ();
      else if (closeEnough (allCommands[10], argv[1], 2)) status = CmdStart    ();
      else if (closeEnough (allCommands[11], argv[1], 2)) status = CmdStop     ();
      else if (closeEnough (allCommands[12], argv[1], 2)) status = CmdTags     ();
      else if (closeEnough (allCommands[13], argv[1], 2)) status = CmdTrack    ();
    }
    else if (matches.size () == 0)
    {
      CmdHelpUsage ();
    }
    else
    {
      std::cout << "timew: '" << argv[1] << "' is not a command. See 'timew help'.\n"
                << "\n"
                << "Did you mean one of these?\n";

      for (const auto& command : matches)
        std::cout << "  " << command << "\n";
    }
  }
  else if (argc == 1)
  {
    status = CmdDefault ();
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////
