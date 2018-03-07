////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2018, Paul Beckingham, Federico Hernandez.
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
#include <Table.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
// Enumerate all extensions.
int CmdExtensions (
  Rules& rules,
  const Extensions& extensions)
{
  Table t;
  t.width (1024);
  t.colorHeader (Color ("underline"));
  t.add ("Extension", true);
  t.add ("Status", true);

  for (auto& ext : extensions.all ())
  {
    File program (ext);

    // Show program name.
    auto row = t.addRow ();
    t.set (row, 0, program.name ());

    // Show extension status.
    std::string perms;
         if (! program.readable ())   perms = "Not readable";
    else if (! program.executable ()) perms = "No executable";
    else                              perms = "Active";

    if (program.is_link ())           perms += " (link)";

    t.set (row, 1, perms);
  }

  Directory extDir (rules.get ("temp.db"));
  extDir += "extensions";

  std::cout << '\n'
            << "Extensions located in:\n"
            << "  " << extDir._data << '\n'
            << '\n'
            << t.render ()
            << '\n';
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
