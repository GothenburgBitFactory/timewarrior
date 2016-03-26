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

#include <cmake.h>
#include <commands.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdReport (
  const std::vector <std::string>& args,
  Rules& rules,
  Database& database,
  Extensions& extensions,
  Log& log)
{
  std::cout << "[report: run a specific report/extension]\n";

  // TODO Load all data.
  // TODO Apply filter.
  // TODO Identify report.

  // TODO Compose Header info.
  //   TODO Configuration.
  //   TODO Exclusions.
  //   TODO Filter.
  //   TODO CLI.
  //   TODO Directory containing *.data files.
  //   TODO cmake.h VERSION

  // TODO Compose JSON.
  // TODO Run report.

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
