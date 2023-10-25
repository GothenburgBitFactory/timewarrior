////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <format.h>
#include <iostream>
#include <shared.h>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
// Іdentify DOM references in cli, provide space-separated results.
int CmdGet (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  auto references = cli.getDomReferences ();
  auto filter = Interval {cli.getRange (), cli.getTags ()};

  std::vector <std::string> results;

  for (auto& reference : references)
  {
    std::string value;

    if (! domGet (database, filter, rules, reference, value))
    {
      throw format ("DOM reference '{1}' is not valid.", reference);
    }

    results.push_back (value);
  }

  std::cout << join (" ", results) << '\n';

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
