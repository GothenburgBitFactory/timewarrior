//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2023, Gothenburg Bit Factory.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included
//  in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
//  https://www.opensource.org/licenses/mit-license.php
//
//////////////////////////////////////////////////////////////////////////////

#include <Extensions.h>
#include <ExtensionsTable.h>
#include <FS.h>
#include <timew.h>

///////////////////////////////////////////////////////////////////////////////
ExtensionsTable::Builder ExtensionsTable::builder ()
{
  return {};
}

///////////////////////////////////////////////////////////////////////////////
ExtensionsTable::Builder& ExtensionsTable::Builder::withExtensions (const Extensions& extensions)
{
  _extensions = extensions;
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
Table ExtensionsTable::Builder::build ()
{
  int terminalWidth = getTerminalWidth ();

  Table table;
  table.width (terminalWidth);
  table.colorHeader (Color ("underline"));
  table.add ("Extension", true);
  table.add ("Status", true);

  for (auto& ext: _extensions.all ())
  {
    File program (ext);

    // Show program name.
    auto row = table.addRow ();
    table.set (row, 0, program.name ());

    // Show extension status.
    std::string status;

    if (! program.readable ())
    {
      status = "Not readable";
    }
    else if (! program.executable ())
    {
      status = "No executable";
    }
    else
    {
      status = "Active";
    }

    if (program.is_link ())
    {
      status += " (link)";
    }

    table.set (row, 1, status);
  }

  return table;
}
