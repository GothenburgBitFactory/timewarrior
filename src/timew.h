////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, Paul Beckingham, Federico Hernandez.
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
Interval                getFilter         (const CLI&);
std::vector <Range>     getHolidays       (const Rules&);
std::vector <Range>     getAllExclusions  (const Rules&, const Range&);
std::vector <Interval>  getAllInclusions  (Database&);
std::vector <Interval>  subset            (const Interval&, const std::vector <Interval>&);
std::vector <Range>     subset            (const Range&, const std::vector <Range>&);
std::vector <Interval>  subset            (const Range&, const std::vector <Interval>&);
std::vector <Interval>  flatten           (const Interval&, const std::vector <Range>&);
std::vector <Range>     merge             (const std::vector <Range>&);
std::vector <Range>     addRanges         (const Range&, const std::vector <Range>&, const std::vector <Range>&);
std::vector <Range>     subtractRanges    (const std::vector <Range>&, const std::vector <Range>&);
Range                   outerRange        (const std::vector <Interval>&);
bool                    matchesFilter     (const Interval&, const Interval&);
Interval                clip              (const Interval&, const Range&);
std::vector <Interval>  getTracked        (Database&, const Rules&, Interval&);
std::vector <Range>     getUntracked      (Database&, const Rules&, Interval&);
Interval                getLatestInterval (Database&);
Range                   getFullDay        (const Datetime&);

// init.cpp
bool lightweightVersionCheck (int, const char**);
void initializeEntities (CLI&);
void initializeDataAndRules (const CLI&, Database&, Rules&);
void initializeExtensions (CLI&, const Rules&, Extensions&);
int dispatchCommand (const CLI&, Database&, Rules&, const Extensions&);

// helper.cpp
Color tagColor (const Rules&, const std::string&);
std::string intervalSummarize (Database&, const Rules&, const Interval&);
bool expandIntervalHint (const std::string&, Range&);
std::string jsonFromIntervals (const std::vector <Interval>&);
Palette createPalette (const Rules&);
std::map <std::string, Color> createTagColorMap (const Rules&, Palette&, const std::vector <Interval>&);
int quantizeToNMinutes (const int, const int);
bool dayIsHoliday (const Rules&, const Datetime&);
bool findHint (const CLI&, const std::string&);
std::string minimalDelta (const Datetime&, const Datetime&);
void validateInterval (Database&, const Rules&, const Interval&);

// log.cpp
void enableDebugMode (bool);
void setDebugIndicator (const std::string&);
void setDebugColor (const Color&);
void debug (const std::string&);

// utiŀ.cpp
std::string osName ();
std::string escape (const std::string&, int);
std::string quoteIfNeeded (const std::string&);

#endif
