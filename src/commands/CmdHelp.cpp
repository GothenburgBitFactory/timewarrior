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

#include <cmake.h>
#include <commands.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <FS.h>
#include <sstream>

#include "additional-help.h"

template <typename T>
std::string join (const T& strings, size_t indent = 0, size_t max_width = 0)
{
  std::stringstream sstr;
  auto it = std::begin (strings);
  auto end = std::end (strings);
  size_t cur = indent + 2; // account for added '{ ' 
  std::string part;
  const std::string sep (" | ");

  while (it != end)
  {
    part = *it;
    ++it;

    // Make sure we leave room for closing " }" or " |"
    if ((max_width > 0) && (cur + part.length () + sep.length ()) > max_width)
    {
      sstr << "\n" << std::string (indent + 1, ' ');
      cur = indent + 1;
      sstr << part;
    }
    else
    {
      sstr << part;
    }
    cur += part.length ();
    
    if (it != end)
    {
      sstr << sep;
      cur += sep.length ();
    }
  }
  return sstr.str ();
}

////////////////////////////////////////////////////////////////////////////////
int CmdHelpUsage (const Extensions& extensions)
{
  std::vector <const char *> concepts;

  // Any commands will take precedence when using `timew help` and therefore,
  // we'll exclude any commands that are also concepts.
  std::set_difference (std::begin (documented_concepts), std::end (documented_concepts),
                       std::begin (documented_commands), std::end (documented_commands),
                       std::back_inserter (concepts),
                       [] (const char *a, const char *b) 
                       {
                         return std::strcmp (a, b) < 0;
                       });

  std::cout << '\n'
            << "Usage: timew [--version]\n"
            << "       timew annotate @<id> [@<id> ...] <annotation>\n"
            << "       timew cancel\n"
            << "       timew config [<name> [<value> | '']]\n"
            << "       timew continue [@<id>] [<date>|<interval>]\n"
            << "       timew day [<interval>] [<tag> ...]\n"
            << "       timew delete @<id> [@<id> ...]\n"
            << "       timew diagnostics\n"
            << "       timew export [<interval>] [<tag> ...]\n"
            << "       timew extensions\n"
            << "       timew gaps [<interval>] [<tag> ...]\n"
            << "       timew get <DOM> [<DOM> ...]\n"
            << "       timew help [<command> | " << join (concepts) << "]\n"
            << "       timew join @<id> @<id>\n"
            << "       timew lengthen @<id> [@<id> ...] <duration>\n"
            << "       timew modify (start|end) @<id> <date>\n"
            << "       timew month [<interval>] [<tag> ...]\n"
            << "       timew move @<id> <date>\n"
            << "       timew [report] <report> [<interval>] [<tag> ...]\n"
            << "       timew shorten @<id> [@<id> ...] <duration>\n"
            << "       timew show\n"
            << "       timew split @<id> [@<id> ...]\n"
            << "       timew start [<date>] [<tag> ...]\n"
            << "       timew stop [<tag> ...]\n"
            << "       timew summary [<interval>] [<tag> ...]\n"
            << "       timew tag @<id> [@<id> ...] <tag> [<tag> ...]\n"
            << "       timew tags [<interval>] [<tag> ...]\n"
            << "       timew track <interval> [<tag> ...]\n"
            << "       timew undo\n"
            << "       timew untag @<id> [@<id> ...] <tag> [<tag> ...]\n"
            << "       timew week [<interval>] [<tag> ...]\n"
            << '\n';

  if (!extensions.all ().empty ())
  {
    std::cout << "Extensions (extensions do not provide help):\n";

    for (auto& ext : extensions.all ())
    {
      std::cout << "       " << Path (ext).name () << '\n';
    }

    std::cout << '\n';
  }

  std::cout << "Additional help:\n"
            << "       timew help <command>\n";
  for (auto concept : concepts)
  {
    std::cout << "       timew help " << concept << '\n';
  }

  std::cout << '\n'
            << "Interval:\n"
            << "       [from] <date>\n"
            << "       [from] <date> to/- <date>\n"
            << "       [from] <date> for <duration>\n"
            << "       <duration> before/after <date>\n"
            << "       <duration> ago\n"
            << "       [for] <duration>\n"
            << '\n'
            << "Tag:\n"
            << "       Word\n"
            << "       'Single Quoted Words'\n"
            << "       \"Double Quoted Words\"\n"
            << "       Escaped\\ Spaces\n"
            << '\n'
            << "Configuration overrides:\n"
            << "       rc.<name>=<value>\n"
            << '\n';

  return 0;
}

int CmdHelp (
  const CLI& cli,
  const Extensions& extensions)
{
  auto words = cli.getWords ();

  if (! words.empty ())
  {
    std::string man_command = "man timew-" + words[0];
    int ret = system (man_command.c_str());
    return (WIFEXITED (ret)) ? WEXITSTATUS (ret) : -1;
  }

  return CmdHelpUsage (extensions);
}

////////////////////////////////////////////////////////////////////////////////
