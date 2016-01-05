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

#ifndef INCLUDED_VIEWTEXT
#define INCLUDED_VIEWTEXT

#include <string>
#include <vector>
#include <Color.h>

class Table
{
public:
  Table ();

  // View specifications.
  void add (const std::string& col, bool alignLeft = true) { _columns.push_back (col); _align.push_back (alignLeft); }
  void width (int width)                                   { _width = width;                                         }
  void leftMargin (int margin)                             { _left_margin = margin;                                  }
  void colorHeader (Color& c)                              { _header = c;                                            }
  void colorOdd (Color& c)                                 { _odd = c;                                               }
  void colorEven (Color& c)                                { _even = c;                                              }
  void intraPadding (int padding)                          { _intra_padding = padding;                               }
  void intraColorOdd (Color& c)                            { _intra_odd = c;                                         }
  void intraColorEven (Color& c)                           { _intra_even = c;                                        }
  void extraPadding (int padding)                          { _extra_padding = padding;                               }
  void extraColorOdd (Color& c)                            { _extra_odd = c;                                         }
  void extraColorEven (Color& c)                           { _extra_even = c;                                        }
  void truncateLines (int n)                               { _truncate_lines = n;                                    }
  void truncateRows (int n)                                { _truncate_rows = n;                                     }
  int lines ()                                             { return _lines;                                          }
  int rows ()                                              { return (int) _data.size ();                             }

  // Data provision.
  int addRow ();
  void set (int, int, const std::string&, Color color = Color::nocolor);
  void set (int, int, int, Color color = Color::nocolor);
  void set (int, int, Color);

  // View rendering.
  std::string render ();

private:
  void measureCell (const std::string&, unsigned int&, unsigned int&) const;
  void renderCell (std::vector <std::string>&, const std::string&, int, bool, Color&) const;

private:
  std::vector <std::vector <std::string>> _data;
  std::vector <std::vector <Color>>       _color;
  std::vector <std::string>               _columns;
  std::vector <bool>                      _align;
  int                                     _width;
  int                                     _left_margin;
  Color                                   _header;
  Color                                   _odd;
  Color                                   _even;
  int                                     _intra_padding;
  Color                                   _intra_odd;
  Color                                   _intra_even;
  int                                     _extra_padding;
  Color                                   _extra_odd;
  Color                                   _extra_even;
  int                                     _truncate_lines;
  int                                     _truncate_rows;
  int                                     _lines;
  int                                     _rows;
};

#endif
////////////////////////////////////////////////////////////////////////////////

