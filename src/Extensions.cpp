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
#include <sstream>
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
std::vector <std::string> Extensions::all () const
{
  return _scripts;
}

////////////////////////////////////////////////////////////////////////////////
std::string Extensions::dump () const
{
  std::stringstream out;

  out << "Extensions\n";
  for (const auto& script : _scripts)
    out << "  " << script << "\n";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string>& Extensions::buildExtensionArgs (std::vector <std::string>& args) const
{
  return args;
}

////////////////////////////////////////////////////////////////////////////////
int Extensions::callExtension (
  const std::string& script,
  const std::vector <std::string>& input,
  std::vector <std::string>& output) const
{
  int status = 0;

  return status;
}

////////////////////////////////////////////////////////////////////////////////
