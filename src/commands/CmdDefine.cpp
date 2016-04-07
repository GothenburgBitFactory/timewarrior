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
#include <commands.h>
#include <shared.h>
#include <format.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// timew define holidays <locale>|none
// timew define holidays [work] <date>
// timew define week <day>[- <day>] [, <day>] ...
// timew define day [<day>] start|end <time>
//
// timew define tag <tag> description <string>
// timew define tag <tag> start|end <date>
// timew define tag <tag> budget <number> <units> per <unit>
// timew define tag <tag> budget <number> <units> total
// timew define tag <tag> overlap
// timew define tag <tag> color <color>
//
int CmdDefine (
  CLI& cli,
  Rules& rules,
  Database& database)
{
  auto words = cli.getWords ();
  if (words.size () >= 1)
  {
    std::vector <std::string> subcommands = {"holidays", "week", "day", "tag"};
    std::vector <std::string> matches;
    autoComplete (words[0], subcommands, matches);

    if (matches.size () != 1)
      throw format ("Unrecognized definition '{1}'.", words[0]);

    if (matches[0] == "holidays")
    {
      std::cout << "# define holidays ...\n";
    }
    else if (matches[0] == "week")
    {
      std::cout << "# define week ...\n";
    }
    else if (matches[0] == "day")
    {
      std::cout << "# define day ...\n";
    }
    else if (matches[0] == "tag")
    {
      std::cout << "# define tag ...\n";
    }
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
