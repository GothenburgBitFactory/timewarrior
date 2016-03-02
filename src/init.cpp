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
  // TODO Load configuration ~/.timewrc || $TIMEWRC.
  // TODO Init database (no data read).

  std::cout << "# Configuration\n";
  for (const auto& name : configuration.all ())
    std::cout << "#   " << name << "=" << configuration[name] << "\n";

  std::cout << "# " << database.dump ();
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
      "help", "clear", "define", "export", "import",
      "report", "start", "stop", "track"
    };

    std::vector <std::string> matches;
    autoComplete (argv[1], allCommands, matches);
    if (matches.size () == 1)
    {
        // These signatures are æxpected to be all different, therefore no
        // command to fn mapping.
             if (closeEnough (allCommands[0], argv[1], 2)) status = CmdHelp   ();
        else if (closeEnough (allCommands[1], argv[1], 2)) status = CmdClear  ();
        else if (closeEnough (allCommands[2], argv[1], 2)) status = CmdDefine (rules);
        else if (closeEnough (allCommands[3], argv[1], 2)) status = CmdExport ();
        else if (closeEnough (allCommands[4], argv[1], 2)) status = CmdImport ();
        else if (closeEnough (allCommands[5], argv[1], 2)) status = CmdReport ();
        else if (closeEnough (allCommands[6], argv[1], 2)) status = CmdStart  ();
        else if (closeEnough (allCommands[7], argv[1], 2)) status = CmdStop   ();
        else if (closeEnough (allCommands[8], argv[1], 2)) status = CmdTrack  ();
    }
    else if (matches.size () == 0)
    {
      // TODO usage.
      std::cout << "# usage\n";
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
