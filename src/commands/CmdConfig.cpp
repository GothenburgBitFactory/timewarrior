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
#include <algorithm>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Note that because this function does not recurse with includes, it therefore
// only sees the top-level settings. This has the desirable effect of adding as
// an override any setting which resides in an imported file.
static bool setConfigVariable (const Rules& rules, std::string name, std::string value, bool confirmation /* = false */)
{
  bool change = false;

  return change;
}

////////////////////////////////////////////////////////////////////////////////
// Removes lines from configuration but leaves comments intact.
//
// Return codes:
//   0 - found and removed
//   1 - found and not removed
//   2 - not found
static int unsetConfigVariable (const Rules& rules, std::string name, bool confirmation /* = false */)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Shows all settings except "temp.*" in a hierarchical form that is ingestible
// by the Rules object. This allows copy/paste.
static void showAllSettings (const Rules& rules)
{
  // Obtain and sort the names. Sorting is critical.
  auto names = rules.all ();
  std::sort (names.begin (), names.end ());

  std::vector <std::string> previous {};
  for (auto& name : rules.all ())
  {
    if (name.substr (0, 5) == "temp.")
      continue;

    auto parts = split (name, '.');
    for (unsigned int i = 0; i < parts.size (); ++i)
    {
      // The last part is special.
      if (i == parts.size () - 1)
      {
        if (previous.size () > 1 &&
            parts.size () == 1)
          std::cout << '\n';

        std::cout << std::string (2 * (parts.size () - 1), ' ')
                  << parts[i]
                  << " = "
                  << rules.get (name)
                  << "\n";
      }
      else
      {
        if (previous.size () <= i ||
            previous[i] != parts[i])
        {
          if (i == 0)
          {
            std::cout << '\n';
            if (rules.isRuleType (parts[0]))
              std::cout << "define ";
          }

          std::cout << std::string (2 * i, ' ')
                    << parts[i]
                    << ":\n";
        }
      }
    }

    previous = parts;
  }
}

////////////////////////////////////////////////////////////////////////////////
// timew config name value       Set name=value
// timew config name ''          Set name=''
// timew config name             Remove name
// timew config                  Show all config
int CmdConfig (
  const CLI& cli,
  Rules& rules)
{
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
        change = setConfigVariable (rules, name, value, confirmation);

      // task config name
      else
      {
        bool found = false;
        rc = unsetConfigVariable (rules, name, confirmation);
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
