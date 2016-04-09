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
#include <Extensions.h>
#include <FS.h>
#include <Timer.h>
#include <shared.h>
#include <sstream>
#include <iostream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
void Extensions::initialize (const std::string& location)
{
  // Scan extension directory.
  Directory d (location);
  if (d.is_directory () &&
      d.readable ())
  {
    _scripts = d.list ();
    std::sort (_scripts.begin (), _scripts.end ());
  }
  else
    throw std::string ("Extension directory not readable: ") + d._data;
}

////////////////////////////////////////////////////////////////////////////////
void Extensions::debug ()
{
  _debug = true;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> Extensions::all () const
{
  return _scripts;
}

////////////////////////////////////////////////////////////////////////////////
int Extensions::callExtension (
  const std::string& script,
  const std::vector <std::string>& input,
  std::vector <std::string>& output) const
{
  if (_debug)
  {
    std::cout << "Extension: Calling " << script << "\n"
              << "Extension: input";

    for (auto& i : input)
      std::cout << "  " << i << "\n";
  }

  auto inputStr = join ("\n", input);

  // Measure time for each hook if running in debug
  int status = 0;
  std::string outputStr;

  if (_debug)
  {
    Timer t ("Extension: execute (" + script + ")");
    status = execute (script, {}, inputStr, outputStr);
  }
  else
  {
    status = execute (script, {}, inputStr, outputStr);
  }

  output = split (outputStr, '\n');

  if (_debug)
    std::cout << "Extension: Completed with status " << status << "\n";

  return status;
}

////////////////////////////////////////////////////////////////////////////////
std::string Extensions::dump () const
{
  std::stringstream out;

  out << "Extensions\n";
  for (auto& script : _scripts)
    out << "  " << script << "\n";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
