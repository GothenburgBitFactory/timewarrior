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
#include <Filter.h>
#include <Timeline.h>
#include <Color.h>

// init.cpp
bool lightweightVersionCheck (int, const char**);
void initializeEntities (CLI&);
void initializeDataAndRules (const CLI&, Database&, Rules&);
void initializeExtensions (const Rules&, Extensions&);
int dispatchCommand (const CLI&, Database&, Rules&, const Extensions&);

// helper.cpp
Color tagColor (const Rules&, const std::string&);
std::string intervalSummarize (const Rules&, const Interval&);
void expandIntervalHint (const std::string&, std::string&, std::string&);
Filter initializeFilterFromCLI (const CLI&);
void populateTimelineFromData (Timeline&, const Rules&, Database&, const Filter&);
Interval getLatestInterval (Timeline&, Database&, const Filter&);

// utiŀ.cpp
std::string osName ();
std::string escape (const std::string&, int);
std::string quoteIfNeeded (const std::string&);

#endif
