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
#include <Filter.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
bool Filter::empty () const
{
  return _range.start ().toEpoch () == 0 &&
         _range.end ().toEpoch ()   == 0 &&
         _tags.size ()           == 0;
}

////////////////////////////////////////////////////////////////////////////////
Daterange Filter::range () const
{
  return _range;
}

////////////////////////////////////////////////////////////////////////////////
void Filter::range (const Daterange& range)
{
  _range = range;
}

////////////////////////////////////////////////////////////////////////////////
std::set <std::string> Filter::tags () const
{
  return _tags;
}

////////////////////////////////////////////////////////////////////////////////
void Filter::tag (const std::string& tag)
{
  if (_tags.find (tag) == _tags.end ())
    _tags.insert (tag);
}

////////////////////////////////////////////////////////////////////////////////
std::string Filter::dump () const
{
  std::stringstream out;

  out << "Filter _range.start '"
      << _range.start ().toEpoch ()
      << "' _range.end '"
      << _range.end ().toEpoch ()
      << "' _tags";

  for (auto& tag : _tags)
    out << " '" << tag << "'";

  out << "\n";
  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
