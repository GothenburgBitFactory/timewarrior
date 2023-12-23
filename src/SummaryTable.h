//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2018, 2023, Gothenburg Bit Factory.
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

#ifndef INCLUDED_SUMMARYTABLE
#define INCLUDED_SUMMARYTABLE

#include <Color.h>
#include <Interval.h>
#include <Range.h>
#include <Table.h>
#include <map>

class SummaryTable
{
  class Builder
  {
  public:
    Builder& withWeekFormat (const std::string&);
    Builder& withDateFormat (const std::string&);
    Builder& withTimeFormat (const std::string&);

    Builder& withAnnotations (bool);
    Builder& withIds (bool, Color);
    Builder& withTags (bool, std::map <std::string, Color>&);
    Builder& withWeeks (bool);
    Builder& withWeekdays (bool);

    Builder& withRange (const Range&);
    Builder& withIntervals (const std::vector <Interval>&);

    Table build ();

  private:
    std::string _week_fmt;
    std::string _date_fmt;
    std::string _time_fmt;

    bool _show_annotations;
    bool _show_ids;
    bool _show_tags;
    bool _show_weekdays;
    bool _show_weeks;

    Range _range;
    std::vector <Interval> _tracked;
    Color _color_id;
    std::map <std::string, Color> _color_tags;
  };

public:
  static Builder builder ();
};

#endif // INCLUDED_SUMMARYTABLE
