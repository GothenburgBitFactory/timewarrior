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
#include <Interval.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (15);

  // bool isStarted () const;
  // bool isEnded () const;
  Interval i1;
  t.is (i1.isStarted (), false, "Interval().isStarted -> false");
  t.is (i1.isEnded (),   false, "Interval().isEnded -> false");

  // void start (Datetime);
  i1.start (Datetime ());
  t.is (i1.isStarted (), true, "Interval(start=now).isStarted -> true");
  t.is (i1.isEnded (),   false, "Interval(start=now).isEnded -> false");

  // void end (Datetime);
  i1.end (Datetime ());
  t.is (i1.isStarted (), true, "Interval(start=now,end=now).isStarted -> true");
  t.is (i1.isEnded (),   true, "Interval(start=now,end=now).isEnded -> true");

  // std::set <std::string> tags () const;
  // void tag (const std::string&);
  Interval i2;
  t.ok (i2.tags () == std::set <std::string> {}, "Interval(tag=) -> {}");
  i2.tag ("foo");
  t.ok (i2.tags () == std::set <std::string> {"foo"}, "Interval(tag=foo) -> {foo}");
  i2.tag ("foo");
  t.ok (i2.tags () == std::set <std::string> {"foo"}, "Interval(tag=foo,foo) -> {foo}");
  i2.tag ("bar");
  t.ok (i2.tags () == std::set <std::string> {"foo", "bar"}, "Interval(tag=foo,bar) -> {foo,bar}");

  // std::string serialize () const;
  Interval i3;
  t.is (i3.serialize (), "inc  -  #", "Interval().serialize -> 'inc  -  #'");
  i3.tag ("foo");
  t.is (i3.serialize (), "inc  -  # foo", "Interval().serialize -> 'inc  -  # foo'");
  i3.tag ("bar");
  t.is (i3.serialize (), "inc  -  # bar foo", "Interval().serialize -> 'inc  -  # bar foo'");
  i3.start (Datetime(1));
  t.is (i3.serialize (), "inc 19700101T000001Z -  # bar foo", "Interval(Datetime(1)).serialize -> 'inc 19700101T000001Z -  # bar foo'");
  i3.end (Datetime(2));
  t.is (i3.serialize (), "inc 19700101T000001Z - 19700101T000002Z # bar foo", "Interval(Datetime(1)).serialize -> 'inc 19700101T000001Z - 19700101T000002Z # bar foo'");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

