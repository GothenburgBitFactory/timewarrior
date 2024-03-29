////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2020, 2022 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <additional-help.h>
#include <algorithm>
#include <commands.h>
#include <iostream>
#include <shared.h>
#include <sys/wait.h>

////////////////////////////////////////////////////////////////////////////////
int CmdHelpUsage (const Extensions& extensions)
{
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
            << "       timew help [<command> | " << join ( " | ", timew_help_concepts) << "]\n"
            << "       timew join @<id> @<id>\n"
            << "       timew lengthen @<id> [@<id> ...] <duration>\n"
            << "       timew modify (start|end) @<id> <date>\n"
            << "       timew month [<interval>] [<tag> ...]\n"
            << "       timew move @<id> <date>\n"
            << "       timew [report] <report> [<interval>] [<tag> ...]\n"
            << "       timew retag @<id> [@<id> ...] <tag> [<tag> ...]\n"
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

  if (! extensions.all ().empty ())
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

  for (auto& concept : timew_help_concepts)
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
