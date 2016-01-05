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
#include <text.h>
#include <utf8.h>

////////////////////////////////////////////////////////////////////////////////
Table::Table ()
: _width (0)
, _left_margin (0)
, _header (0)
, _odd (0)
, _even (0)
, _intra_padding (1)
, _intra_odd (0)
, _intra_even (0)
, _extra_padding (0)
, _extra_odd (0)
, _extra_even (0)
, _truncate_lines (0)
, _truncate_rows (0)
, _lines (0)
, _rows (0)
{
}

////////////////////////////////////////////////////////////////////////////////
int Table::addRow ()
{
  _data.push_back (std::vector <std::string> (_columns.size (), ""));
  _color.push_back (std::vector <Color> (_columns.size (), Color::nocolor));
  return _data.size () - 1;
}

////////////////////////////////////////////////////////////////////////////////
void Table::set (int row, int col, const std::string& value, Color color)
{
  _data[row][col] = value;

  if (color.nontrivial ())
    _color[row][col] = color;
}

////////////////////////////////////////////////////////////////////////////////
void Table::set (int row, int col, int value, Color color)
{
  std::string string_value = format (value);
  _data[row][col] = string_value;

  if (color.nontrivial ())
    _color[row][col] = color;
}

////////////////////////////////////////////////////////////////////////////////
void Table::set (int row, int col, Color color)
{
  if (color.nontrivial ())
    _color[row][col] = color;
}

////////////////////////////////////////////////////////////////////////////////
std::string Table::render ()
{
  // Determine minimal, ideal column widths.
  std::vector <int> minimal;
  std::vector <int> ideal;
  for (unsigned int col = 0; col < _columns.size (); ++col)
  {
    // Headers factor in to width calculations.
    unsigned int global_min = utf8_width (_columns[col]);
    unsigned int global_ideal = global_min;

    for (unsigned int row = 0; row < _data.size (); ++row)
    {
      // Determine minimum and ideal width for this column.
      unsigned int min = 0;
      unsigned int ideal = 0;
      measureCell (_data[row][col], min, ideal);

      if (min   > global_min)   global_min = min;
      if (ideal > global_ideal) global_ideal = ideal;
    }

    minimal.push_back (global_min);
    ideal.push_back (global_ideal);
  }

  // Sum the minimal widths.
  int sum_minimal = 0;
  for (auto& c : minimal)
    sum_minimal += c;

  // Sum the ideal widths.
  int sum_ideal = 0;
  for (auto& c : ideal)
    sum_ideal += c;

  // Calculate final column widths.
  int overage = _width
              - _left_margin
              - (2 * _extra_padding)
              - ((_columns.size () - 1) * _intra_padding);

  std::vector <int> widths;
  if (sum_ideal <= overage)
    widths = ideal;
  else if (sum_minimal > overage || overage < 0)
    widths = minimal;
  else if (overage > 0)
  {
    widths = minimal;
    overage -= sum_minimal;

    // Spread 'overage' among columns where width[i] < ideal[i]
    while (overage)
    {
      for (unsigned int i = 0; i < _columns.size () && overage; ++i)
      {
        if (widths[i] < ideal[i])
        {
          ++widths[i];
          --overage;
        }
      }
    }
  }

  // Compose column headers.
  unsigned int max_lines = 0;
  std::vector <std::vector <std::string>> headers;
  for (unsigned int c = 0; c < _columns.size (); ++c)
  {
    headers.push_back ({});
    renderCell (headers[c], _columns[c], widths[c], _align[c], _header);

    if (headers[c].size () > max_lines)
      max_lines = headers[c].size ();
  }

  // Output string.
  std::string out;
  _lines = 0;

  // Render column headers.
  std::string left_margin = std::string (_left_margin, ' ');
  std::string extra       = std::string (_extra_padding, ' ');
  std::string intra       = std::string (_intra_padding, ' ');

  std::string extra_odd   = _extra_odd.colorize  (extra);
  std::string extra_even  = _extra_even.colorize (extra);
  std::string intra_odd   = _intra_odd.colorize  (intra);
  std::string intra_even  = _intra_even.colorize (intra);

  for (unsigned int i = 0; i < max_lines; ++i)
  {
    out += left_margin + extra;

    for (unsigned int c = 0; c < _columns.size (); ++c)
    {
      if (c)
        out += intra;

      if (headers[c].size () < max_lines - i)
        out += _header.colorize (std::string (widths[c], ' '));
      else
        out += headers[c][i];
    }

    out += extra;

    // Trim right.
    out.erase (out.find_last_not_of (" ") + 1);
    out += "\n";

    // Stop if the line limit is exceeded.
    if (++_lines >= _truncate_lines && _truncate_lines != 0)
      return out;
  }

  // Compose, render columns, in sequence.
  _rows = 0;
  std::vector <std::vector <std::string>> cells;
  for (unsigned int row = 0; row < _data.size (); ++row)
  {
    max_lines = 0;

    // Alternate rows based on |s % 2|
    bool odd = (row % 2) ? true : false;
    Color row_color = odd ? _odd : _even;

    // TODO row_color.blend (provided color);
    // TODO Problem: colors for columns are specified, not rows,
    //      therefore there are only cell colors, not intra colors.

    Color cell_color;
    for (unsigned int col = 0; col < _columns.size (); ++col)
    {
      cell_color = row_color;
      cell_color.blend (_color[row][col]);

      cells.push_back (std::vector <std::string> ());
      renderCell (cells[col], _data[row][col], widths[col], _align[col], cell_color);

      if (cells[col].size () > max_lines)
        max_lines = cells[col].size ();
    }

    for (unsigned int i = 0; i < max_lines; ++i)
    {
      out += left_margin + (odd ? extra_odd : extra_even);

      for (unsigned int col = 0; col < _columns.size (); ++col)
      {
        if (col)
        {
          if (row_color.nontrivial ())
            out += row_color.colorize (intra);
          else
            out += (odd ? intra_odd : intra_even);
        }

        if (i < cells[col].size ())
          out += cells[col][i];
        else
        {
          cell_color = row_color;
          cell_color.blend (_color[row][col]);
          out += cell_color.colorize (std::string (widths[col], ' '));
        }
      }

      out += (odd ? extra_odd : extra_even);

      // Trim right.
      out.erase (out.find_last_not_of (" ") + 1);
      out += "\n";

      // Stop if the line limit is exceeded.
      if (++_lines >= _truncate_lines && _truncate_lines != 0)
        return out;
    }

    cells.clear ();

    // Stop if the row limit is exceeded.
    if (++_rows >= _truncate_rows && _truncate_rows != 0)
      return out;
  }

  return out;
}

////////////////////////////////////////////////////////////////////////////////
void Table::measureCell (
  const std::string& data,
  unsigned int& minimum,
  unsigned int& maximum) const
{
  std::string stripped = Color::strip (data);
  maximum = longestLine (stripped);
  minimum = longestWord (stripped);
}

////////////////////////////////////////////////////////////////////////////////
void Table::renderCell (
  std::vector <std::string>& lines,
  const std::string& value,
  int width,
  bool alignLeft,
  Color& color) const
{
  std::vector <std::string> raw;
  wrapText (raw, value, width, false);

  for (auto& line : raw)
    if (alignLeft)
      lines.push_back (
        color.colorize (
          leftJustify (line, width)));
    else
      lines.push_back (
        color.colorize (
          rightJustify (line, width)));
}

////////////////////////////////////////////////////////////////////////////////

