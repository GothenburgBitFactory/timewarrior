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
  explicit Chart (ChartConfig configuration);

  std::string render (const Interval&, const std::vector <Interval>&, const std::vector <Range>&, const std::map <Datetime, std::string>&, const std::map <std::string, Color>&, const Color&, const Color&, const Color&, const Color&, bool, bool, bool, bool, bool, bool, bool, int, int, int);

private:
  unsigned long getIndentSize ();

  std::pair <int, int> determineHourRange (const Interval&, const std::vector <Interval>&);

  std::string renderAxis (int, int, const Color&, const Color&, int, bool);

  std::string renderMonth (const Datetime&, const Datetime&);

  std::string renderWeek (const Datetime&, const Datetime&);

  std::string renderWeekday (Datetime&, const Color&);

  std::string renderDay (Datetime&, const Color&);

  Color getDayColor (const Datetime&, const Datetime&, const std::map <Datetime, std::string>&, const Color&, const Color&);

  std::string renderTotal (time_t);

  std::string renderSubTotal (time_t, const std::string&);

  void renderExclusionBlocks (std::vector<Composite>&, const Datetime&, int, int, const std::vector<Range>&, int, int, const Color&, const Color&, bool);

  void renderInterval (std::vector<Composite>&, const Datetime&, const Interval&, const std::map<std::string, Color>&, int, time_t&, bool, int, int);

  std::string renderHolidays (const std::map <Datetime, std::string>&);

  std::string renderSummary (const std::string&, const Interval&, const std::vector <Range>&, const std::vector <Interval>&, bool);

  const bool with_label_month;
  const bool with_label_week;
  const bool with_label_weekday;
  const bool with_label_day;
};

#endif
