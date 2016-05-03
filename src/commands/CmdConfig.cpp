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
#include <shared.h>
#include <format.h>
#include <timew.h>
#include <stack>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
static bool setConfigVariable (std::string name, std::string value, bool confirmation /* = false */)
{
  bool change = false;

  return change;
}

////////////////////////////////////////////////////////////////////////////////
static int unsetConfigVariable (std::string name, bool confirmation /* = false */)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// a.b.c.d = 1
// a.b.d = 2
// a.b.e = 3
// b.d.e = 4
// f = 5
//
// define a:
//   b:
//     c:
//       d = 1
//     d = 2
//     e = 3
//
// define b:
//   d:
//     e = 4
//
// f = 5
static void showAllSettings (const Rules& rules)
{
  for (auto& name : rules.all ())
    std::cout << name << " = " << rules.get (name) << "\n";
}

////////////////////////////////////////////////////////////////////////////////
int CmdConfig (
  const CLI& cli,
  Rules& rules)
{
  // Determine form:
  //      timew config name value       Set name=value
  //      timew config name ''          Set name=''
  //      timew config name             Remove name
  // TODO timew config                  Show all config

  int rc = 0;

  // Get the command line args that are not binary, ext or command.
  auto words = cli.getWords ();

  // Support:
  //   task config name value    # set name to value
  //   task config name ""       # set name to blank
  //   task config name          # remove name
  if (words.size ())
  {
    bool confirmation = rules.getBoolean ("confirmation");
    std::string name = words[0];
    std::string value = "";

    // Join the remaining words into config variable's value
    if (words.size () > 1)
    {
      for (unsigned int i = 1; i < words.size (); ++i)
      {
        if (i > 1)
          value += " ";

        value += words[i];
      }
    }

    if (name != "")
    {
      bool change = false;

      // task config name value
      // task config name ""
      if (words.size () > 1)
        change = setConfigVariable(name, value, confirmation);

      // task config name
      else
      {
        bool found = false;
        rc = unsetConfigVariable(name, confirmation);
        if (rc == 0)
        {
          change = true;
          found = true;
        }
        else if (rc == 1)
          found = true;

        if (!found)
          throw format ("No entry named '{1}' found.", name);
      }

      if (change)
        std::cout << "Config file " << rules.file () << " modified.\n";
      else
        std::cout << "No changes made.\n";
    }
    else
      showAllSettings (rules);
  }
  else
    showAllSettings (rules);

  return rc;
}

////////////////////////////////////////////////////////////////////////////////
