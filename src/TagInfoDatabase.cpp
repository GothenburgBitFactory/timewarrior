////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include "timew.h"

///////////////////////////////////////////////////////////////////////////////
// Increment tag count
// If it does not exist, a new entry is created
//
// Returns the previous tag count, -1 if it did not exist
//
int TagInfoDatabase::incrementTag (const std::string& tag)
{
  auto search = _tagInformation.find (tag);

  if (search == _tagInformation.end ())
  {
    add (tag, TagInfo {1});

    return -1;
  }

  _is_modified = true;
  return search->second.increment ();
}

///////////////////////////////////////////////////////////////////////////////
// Decrement tag count
//
// Returns the new tag count
//
int TagInfoDatabase::decrementTag (const std::string& tag)
{
  auto search = _tagInformation.find (tag);

  if (search == _tagInformation.end ())
  {
    throw format ("Trying to decrement non-existent tag '{1}'", tag);
  }

  _is_modified = true;
  return search->second.decrement ();
}

///////////////////////////////////////////////////////////////////////////////
// Add tag to database
//
void TagInfoDatabase::add (const std::string& tag, const TagInfo& tagInfo)
{
  _is_modified = true;
  _tagInformation.emplace (tag, tagInfo);
}

///////////////////////////////////////////////////////////////////////////////
// Return the current set of tag names
//
std::set <std::string> TagInfoDatabase::tags () const
{
  std::set <std::string> tags; 

  for (auto& item : _tagInformation)
  {
    tags.insert (item.first);
  }

  return tags;
}

bool TagInfoDatabase::is_modified () const
{
  return _is_modified;
}

void TagInfoDatabase::clear_modified ()
{
  _is_modified = false;
}

///////////////////////////////////////////////////////////////////////////////
std::string TagInfoDatabase::toJson ()
{
  std::stringstream json;
  bool first = true;

  json << "{";

  for (auto& pair : _tagInformation)
  {
    auto tagInfo = pair.second;

    if (tagInfo.hasCount ())
    {
      json << (first ? "" : ",")
         << "\"" << escape(pair.first, '"') << "\":"
         << tagInfo.toJson ();

      first = (first ? false : first);
    }
  }

  json << "}";

  return json.str ();
}
