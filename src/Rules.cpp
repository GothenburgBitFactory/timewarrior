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
#include <Rules.h>
#include <FS.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
// Nested files are supported, with the following construct:
//   import /absolute/path/to/file
void Rules::load (const std::string& file, int nest /* = 1 */)
{
  if (nest > 10)
    throw std::string ("Rules files may only be nested to 10 levels.");

  // First time in, load the default values.
  if (nest == 1)
  {
    // This is where defaults would be set.
    _original_file = File (file);
  }

  // Read the file, then parse the contents.
  std::string contents;
  if (File::read (file, contents) && contents.length ())
    parse (contents, nest);
}

////////////////////////////////////////////////////////////////////////////////
// define r:
//    name value
std::string Rules::get (const std::string& rule, const std::string& name) const
{
  return ""; // TODO Replace
}

////////////////////////////////////////////////////////////////////////////////
std::string Rules::dump () const
{
  std::stringstream out;
  out << "Rules\n"
      << "  _original_file " << _original_file
      << "\n";

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
void Rules::parse (const std::string& input, int nest /* = 1 */)
{
}

////////////////////////////////////////////////////////////////////////////////
