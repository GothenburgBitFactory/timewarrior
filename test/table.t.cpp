////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2006 - 2016, Paul Beckingham, Federico Hernandez.
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
#include <Table.h>
#include <iostream>
//#include <stdlib.h>
//#include <unistd.h>
//#include <stdio.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (1);

  try
  {
    // Create colors.
    Color header_color (Color (Color::yellow, Color::nocolor, false, false, false));
    Color odd_color ("on gray1");
    Color even_color ("on gray0");

    // Now render a string-only grid.
    Color single_cell ("bold white on red");

    Table t1;
    t1.width (80);
    t1.leftMargin (4);
    t1.extraPadding (0);
    t1.intraPadding (1);
    t1.colorHeader (header_color);
    t1.colorOdd (odd_color);
    t1.colorEven (even_color);
    t1.intraColorOdd (odd_color);
    t1.intraColorEven (even_color);

//    t1.add (Column::factory ("string", "One"));
//    t1.add (Column::factory ("string", "Two"));
//    t1.add (Column::factory ("string", "Three"));
    t1.add ("Header1", true);
    t1.add ("Header2", true);
    t1.add ("Header3", false);

    int row = t1.addRow ();
    t1.set (row, 0, "top left");
    t1.set (row, 1, "top center");
    t1.set (row, 2, "top right");

    row = t1.addRow ();
    t1.set (row, 0, "bottom left", single_cell);
    t1.set (row, 1, "bottom center, containing sufficient text that "
                             "wrapping will occur because it exceeds all "
                             "reasonable values for default width.  Even in a "
                             "very wide terminal window.  Just look at the "
                             "lengths we must go to, to get passing unit tests "
                             "and not flaky tests.");
    t1.set (row, 2, "bottom right");

    std::cout << t1.render ();
    t.ok (t1.lines () > 4, "Table::lines > 4");
  }

  catch (const std::string& e)
  {
    t.fail ("Exception thrown.");
    t.diag (e);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
