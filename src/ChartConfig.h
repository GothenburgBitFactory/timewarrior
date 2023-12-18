////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019, 2022, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_CHARTCONFIG
#define INCLUDED_CHARTCONFIG

#include <Color.h>
#include <Datetime.h>
#include <map>

class ChartConfig
{
public:
  Datetime reference_datetime;
  bool with_label_month;
  bool with_label_week;
  bool with_label_weekday;
  bool with_label_day;
  bool with_ids;
  bool with_summary;
  bool with_holidays;
  bool with_totals;
  bool with_internal_axis;
  bool show_intervals;
  bool determine_hour_range;
  int minutes_per_char;
  int spacing;
  int num_lines;
  Color color_today;
  Color color_holiday;
  Color color_label;
  Color color_exclusion;
  std::map <std::string, Color> tag_colors;
};

#endif
