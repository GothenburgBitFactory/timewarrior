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
#include <timew.h>
#include <Table.h>
#include <Color.h>
#include <set>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdTags (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Create a filter, with no default range.
  auto filter = getFilter (cli);

  // Generate a unique, ordered list of tags.
  std::set <std::string> tags;
  for (const auto& interval : getTracked (database, rules, filter))
    for (auto& tag : interval.tags ())
      tags.insert (tag);

  // Shows all tags.
  if (! tags.empty ())
  {
    Table t;
    t.width (1024);
    t.colorHeader (Color ("underline"));
    t.add ("Tag");
    t.add ("Description");
    // TODO Show all tag metadata.

    for (auto& tag : tags)
    {
      auto row = t.addRow ();
      t.set (row, 0, tag, tagColor (rules, tag));

      auto name = std::string ("tags.") + tag + ".description";
      t.set (row, 1, rules.has (name) ? rules.get (name) : "-");
    }

    std::cout << '\n'
              << t.render ()
              << '\n';
  }
  else
  {
    if (rules.getBoolean ("verbose"))
      std::cout << "No data found.\n";
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
