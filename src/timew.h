////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_TIMEW
#define INCLUDED_TIMEW

#include <CLI.h>
#include <Database.h>
#include <Rules.h>
#include <Extensions.h>
#include <Interval.h>
#include <Exclusion.h>
#include <Palette.h>
#include <Color.h>

// data.cpp
std::vector <Range>     getHolidays       (const Rules&);
std::vector <Range>     getAllExclusions  (const Rules&, const Range&);
std::vector <Interval>  getIntervalsByIds (Database&, const Rules&, const std::set <int>&);
std::vector <Interval>  subset            (const Interval&, const std::vector <Interval>&);
std::vector <Range>     subset            (const Range&, const std::vector <Range>&);
std::vector <Interval>  subset            (const Range&, const std::vector <Interval>&);
void                    flattenDatabase   (Database&, const Rules&);
std::vector <Interval>  flatten           (const Interval&, const std::vector <Range>&);
std::vector <Range>     merge             (const std::vector <Range>&);
std::vector <Range>     addRanges         (const Range&, const std::vector <Range>&, const std::vector <Range>&);
std::vector <Range>     subtractRanges    (const std::vector <Range>&, const std::vector <Range>&);
Range                   outerRange        (const std::vector <Interval>&);
bool                    matchesRange      (const Interval&, const Range&);
bool                    matchesFilter     (const Interval&, const Interval&);
Interval                clip              (const Interval&, const Range&);
std::vector <Interval>  getTracked        (Database&, const Rules&, Interval&);
std::vector <Range>     getUntracked      (Database&, const Rules&, Interval&);
Interval                getLatestInterval (Database&);
Range                   getFullDay        (const Datetime&);

// validate.cpp
void autoFill (const Rules&, Database&, Interval&);
void validate (const CLI& cli, const Rules& rules, Database&, Interval&);

// init.cpp
bool lightweightVersionCheck (int, const char**);
void initializeEntities (CLI&);
void initializeDataJournalAndRules (const CLI&, Database&, Journal&, Rules&);
void initializeExtensions (CLI&, const Rules&, Extensions&);
int dispatchCommand (const CLI&, Database&, Journal&, Rules&, const Extensions&);

// helper.cpp
Color intervalColor (const std::set <std::string>&, const std::map <std::string, Color>&);
Color tagColor (const Rules&, const std::string&);
std::string intervalSummarize (Database&, const Rules&, const Interval&);
bool expandIntervalHint (const std::string&, Range&);
std::string jsonFromIntervals (const std::vector <Interval>&);
Palette createPalette (const Rules&);
std::map <std::string, Color> createTagColorMap (const Rules&, Palette&, const std::vector <Interval>&);
int quantizeToNMinutes (int, int);

bool findHint (const CLI&, const std::string&);
std::string minimalDelta (const Datetime&, const Datetime&);
std::vector <Interval> getOverlaps (Database&, const Rules&, const Interval&);

// log.cpp
void enableDebugMode (bool);
void setDebugIndicator (const std::string&);
void setDebugColor (const Color&);
void debug (const std::string&);

// utiŀ.cpp
std::string escape (const std::string&, int);
std::string quoteIfNeeded (const std::string&);
std::string join(const std::string& glue, const std::set <std::string>& array);
std::string joinQuotedIfNeeded(const std::string& glue, const std::set <std::string>& array);
std::string joinQuotedIfNeeded(const std::string& glue, const std::vector <std::string>& array);

// dom.cpp
bool domGet (Database&, Interval&, const Rules&, const std::string&, std::string&);

#endif
