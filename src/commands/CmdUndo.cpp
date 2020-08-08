////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <commands.h>
#include <timew.h>
#include <iostream>
#include <format.h>
#include <IntervalFactory.h>

static void undoIntervalAction(UndoAction& action, Database& database)
{
  Interval before = IntervalFactory::fromJson (action.getBefore ());
  Interval after = IntervalFactory::fromJson (action.getAfter ());

  database.modifyInterval (after, before, false);
}

static void undoConfigAction (UndoAction& action, Rules &rules, Journal& journal)
{
  const std::string& before = action.getBefore ();

  if (before.empty ()) {
    const std::string& after = action.getAfter ();
    auto pos = after.find (' ');
    std::string name = after.substr (0, pos);

    Rules::unsetConfigVariable (journal, rules, name, false);
  }
  else
  {
    auto pos = before.find ('=');
    std::string name = before.substr (0, pos-1);
    std::string value = before.substr (pos+1, before.length ());

    Rules::setConfigVariable (journal, rules, name, value, false);
  }
}

////////////////////////////////////////////////////////////////////////////////
int CmdUndo (Rules& rules, Database& database, Journal& journal)
{
  const bool verbose = rules.getBoolean ("verbose");

  Transaction transaction = journal.popLastTransaction ();

  std::vector <UndoAction> actions = transaction.getActions ();

  if (actions.empty ())
  {
    // No (more) undoing...
    if (verbose)
    {
      std::cout << "Nothing to undo." << std::endl;
    }
  }
  else
  {
    for (auto& action : actions)
    {
      // Select database...
      std::string type = action.getType ();

      // Rollback action...
      if (type == "interval")
      {
        undoIntervalAction (action, database);
      }
      else if (type == "config")
      {
        undoConfigAction (action, rules, journal);
      }
      else
      {
        throw format ("Unknown undo action type '{1}'", type);
      }
    }

    if (verbose)
    {
      std::cout << "Undo" << std::endl;
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
