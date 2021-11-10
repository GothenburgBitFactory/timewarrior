////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <IntervalFilterAllWithTags.h>
#include <Interval.h>

IntervalFilterAllWithTags::IntervalFilterAllWithTags (std::set <std::string> tags, const bool complexFiltering): _tags (std::move(tags)), _complexFiltering (complexFiltering)
{}

bool IntervalFilterAllWithTags::accepts (const Interval& interval)
{
  if (_complexFiltering) {
    // TODO: WIP, This does not fully implement boolean logic.
    // Check if OR exists.
    bool isOr = false;
    for (auto& tag : _tags)
    {
      if (tag == "OR")
      {
        isOr = true;
        break;
      }
    }

    for (auto& tag : _tags)
    {
      // Process negation
      bool isNegative = tag[0] == '-';
      std::string tagName = isNegative ? tag.substr(1) : tag;
      bool hasTag = interval.hasTag(tagName);
      if (isNegative) hasTag = !hasTag;

      // Return true at the first sign of a tag, if we're using OR logic.
      if (isOr && hasTag)
      {
        return true;
      }

      // Return false at the first sign of miss, if we're using AND logic.
      if (!isOr && !hasTag)
      {
        return false;
      }
    }

    // OR logic : Nothing triggered the match, so this interval does not match.
    // AND logic: Nothing triggered the miss, so this interval matches.
    return isOr ? false : true;
  }
  else {
    // Old filtering AND logic
    for (auto& tag : _tags)
    {
      if (! interval.hasTag (tag))
      {
        return false;
      }
    }

    return true;
  }
}
