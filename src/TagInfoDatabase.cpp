////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <TagInfoDatabase.h>
#include <format.h>
#include <TagInfo.h>

///////////////////////////////////////////////////////////////////////////////
// Increment tag count
// If it does not exist, a new entry is created
//
// Returns the previous tag count, -1 if it did not exist
//
int TagInfoDatabase::incrementTag (const std::string &tag)
{
  auto search = _tagInformation.find (tag);

  if (search == _tagInformation.end ())
  {
    _tagInformation.emplace (tag, TagInfo (1));

    return -1;
  }

  return search->second.increment ();
}

///////////////////////////////////////////////////////////////////////////////
// Decrement tag count
//
// Returns the new tag count
//
int TagInfoDatabase::decrementTag (const std::string &tag)
{
  auto search = _tagInformation.find (tag);

  if (search == _tagInformation.end ())
  {
    throw format ("Trying to remove non-existent tag '{}'", tag);
  }

  return search->second.decrement ();
}
