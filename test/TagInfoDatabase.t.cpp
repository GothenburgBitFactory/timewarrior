////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (8);

  {
    TagInfoDatabase tagInfoDatabase{};

    tagInfoDatabase.add ("foo", TagInfo{1});
    tagInfoDatabase.add ("bar", TagInfo{2});

    t.is (tagInfoDatabase.incrementTag ("baz"), -1, "Insertion of new tag returns -1");
    t.is (tagInfoDatabase.incrementTag ("foo"), 1, "Increment of existing tag returns previous count");
    t.is (tagInfoDatabase.decrementTag ("bar"), 1, "Decrement of existing tag returns new count");
  }

  {
    TagInfoDatabase tagInfoDatabase{};

    t.is (tagInfoDatabase.toJson (),
          "{\n}",
          "JSON output for empty database");

    tagInfoDatabase.incrementTag ("foo");
    t.is (tagInfoDatabase.toJson (),
          "{\n  \"foo\":{\"count\":1}\n}",
          "JSON output for single entry");

    tagInfoDatabase.incrementTag("bar");
    tagInfoDatabase.incrementTag("baz");
    tagInfoDatabase.incrementTag("foo");

    t.is (tagInfoDatabase.toJson (),
          "{\n  \"bar\":{\"count\":1},\n  \"baz\":{\"count\":1},\n  \"foo\":{\"count\":2}\n}",
          "JSON output for multiple entries");

    tagInfoDatabase.decrementTag("baz");

    t.is (tagInfoDatabase.toJson (),
          "{\n  \"bar\":{\"count\":1},\n  \"foo\":{\"count\":2}\n}",
          "Tags with count 0 are purged from database");
  }

  {
    TagInfoDatabase tagInfoDatabase{};

    try
    {
      tagInfoDatabase.decrementTag ("xyz");
      t.fail ("Decrement of non-existent tag throws an exception");
    }
    catch (...)
    {
      t.pass ("Decrement of non-existent tag throws an exception");
    }
  }

  return 0;
}



