////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_CHART
#define INCLUDED_CHART

#include <Composite.h>
#include <Interval.h>
#include <map>
#include "ChartConfig.h"

class Chart
{
public:
  explicit Chart (const ChartConfig& configuration);

  std::string render (const Interval&, const std::vector <Interval>&, const std::vector <Range>&, const std::map <Datetime, std::string>&);

private:
  std::string renderAxis (int, int);
  std::string renderDay (Datetime&, const Color&);
  std::string renderHolidays (const std::map <Datetime, std::string>&);
  std::string renderMonth (const Datetime&, const Datetime&);
  std::string renderSubTotal (time_t, const std::string&);
  std::string renderSummary (const std::string&, const Interval&, const std::vector <Range>&, const std::vector <Interval>&);
  std::string renderTotal (time_t);
  std::string renderWeek (const Datetime&, const Datetime&);
  std::string renderWeekday (Datetime&, const Color&);

  void renderExclusionBlocks (std::vector <Composite>&, const Datetime&, int, int, const std::vector <Range>&);
  void renderInterval (std::vector<Composite>&, const Datetime&, const Interval&, int, time_t&);

  unsigned long getIndentSize ();

  std::pair <int, int> determineHourRange (const Interval&, const std::vector <Interval>&);

  Color getDayColor (const Datetime&, const std::map <Datetime, std::string>&);

  const Datetime reference_datetime;
  const bool with_label_month;
  const bool with_label_week;
  const bool with_label_weekday;
  const bool with_label_day;
  const bool with_ids;
  const bool with_summary;
  const bool with_holidays;
  const bool with_totals;
  const bool with_internal_axis;
  const bool show_intervals;
  const bool determine_hour_range;
  const int minutes_per_char;
  const int spacing;
  const int num_lines;
  const Color color_today;
  const Color color_holiday;
  const Color color_label;
  const Color color_exclusion;
  const std::map<std::string, Color> tag_colors;
  const int cell_width;
};

#endif
