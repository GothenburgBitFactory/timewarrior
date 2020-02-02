////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <FS.h>
#include <JSON.h>
#include <shared.h>
#include <format.h>
#include <timew.h>
#include <commands.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// timew config name value       Set name=value
// timew config name ''          Set name=''
// timew config name             Remove name
// timew config                  Show all config
int CmdConfig (
  const CLI& cli,
  Rules& rules,
  Journal& journal)
{
  int rc = 0;

  // Get the command line args that are not binary, ext or command.
  auto words = cli.getWords ();

  // Support:
  //   timew config name value    # set name to value
  //   timew config name ""       # set name to blank
  //   timew config name          # remove name
  if (words.empty ())
  {
    return CmdShow (rules);
  }

  auto verbose = rules.getBoolean ("verbose");

  bool confirmation = rules.getBoolean ("confirmation");
  std::string name = words[0];
  std::string value;

  if (name.empty ()) // is this possible?
  {
    return CmdShow (rules);
  }

  bool change = false;

  journal.startTransaction ();

  // timew config name value
  // timew config name ""
  if (words.size () > 1)
  {
    // Join the remaining words into config variable's value
    for (unsigned int i = 1; i < words.size (); ++i)
    {
      if (i > 1)
      {
        value += " ";
      }

      value += words[i];
    }

    change = Rules::setConfigVariable (journal, rules, name, value, confirmation);

    if (!change)
    {
      rc = 1;
    }
  }
  // timew config name
  else
  {
    bool found = false;
    rc = Rules::unsetConfigVariable (journal, rules, name, confirmation);
    if (rc == 0)
    {
      change = true;
      found = true;
    }
    else if (rc == 1)
    {
      found = true;
    }

    if (!found)
    {
      throw format ("No entry named '{1}' found.", name);
    }
  }

  journal.endTransaction ();

  if (verbose)
  {
    if (change)
    {
      std::cout << "Config file " << rules.file () << " modified.\n";
    }
    else
    {
      std::cout << "No changes made.\n";
    }
  }

  return rc;
}

////////////////////////////////////////////////////////////////////////////////
