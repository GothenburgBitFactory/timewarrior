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

class Chart
{
public:
  Chart() = default;

  static void render (const Interval&, const std::vector <Interval>&, const std::vector <Range>&, const std::map <Datetime, std::string>&, const std::map <std::string, Color>&, const Color&, const Color&, const Color&, const Color&, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, int, int, int);

  static unsigned long getIndentSize (bool, bool, bool, bool);

  static std::pair <int, int> determineHourRange (const Interval&, const std::vector <Interval>&);

  static std::string renderAxis (int, int, const Color&, const Color&, int, bool);

  static std::string renderMonth (const Datetime&, const Datetime&);

  static std::string renderWeek (const Datetime&, const Datetime&);

  static std::string renderWeekday (Datetime&, const Color&);

  static std::string renderDay (Datetime&, const Color&);

  static Color getDayColor (const Datetime&, const Datetime&, const std::map <Datetime, std::string>&, const Color&, const Color&);

  static std::string renderTotal (time_t);

  static std::string renderSubTotal (time_t, const std::string&);

  static void renderExclusionBlocks (std::vector<Composite>&, const Datetime&, int, int, const std::vector<Range>&, int, int, const Color&, const Color&, bool);

  static void renderInterval (std::vector<Composite>&, const Datetime&, const Interval&, const std::map<std::string, Color>&, int, time_t&, bool, int, int);

  static std::string renderHolidays (const std::map <Datetime, std::string>&);

  static std::string renderSummary (const std::string&, const Interval&, const std::vector <Range>&, const std::vector <Interval>&, bool);
};

#endif
