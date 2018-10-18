////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <cmake.h>
#include <Interval.h>
#include <IntervalFactory.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (64);

  // bool is_started () const;
  // bool is_ended () const;
  Interval i1;
  t.is (i1.empty (),      true,  "Interval().empty -> true");
  t.is (i1.is_started (), false, "Interval().is_started -> false");
  t.is (i1.is_ended (),   false, "Interval().is_ended -> false");

  // void start (Datetime);
  i1.start = Datetime ();
  t.is (i1.empty (),      false, "Interval().empty -> false");
  t.is (i1.is_started (), true,  "Interval(start=now).is_started -> true");
  t.is (i1.is_ended (),   false, "Interval(start=now).is_ended -> false");

  // void end (Datetime);
  i1.end = Datetime ();
  t.is (i1.is_started (), true, "Interval(start=now,end=now).is_started -> true");
  t.is (i1.is_ended (),   true, "Interval(start=now,end=now).is_ended -> true");

  // std::set <std::string> tags () const;
  // void tag (const std::string&);
  Interval i2;
  t.ok (i2.tags ().empty (), "Interval(tag=) -> {}");
  i2.tag ("foo");
  t.ok (i2.tags () == std::set <std::string> {"foo"}, "Interval(tag=foo) -> {foo}");
  i2.tag ("foo");  // Duplicate
  t.ok (i2.tags () == std::set <std::string> {"foo"}, "Interval(tag=foo,foo) -> {foo}");
  i2.tag ("bar");
  t.ok (i2.tags () == std::set <std::string> {"foo", "bar"}, "Interval(tag=foo,bar) -> {foo,bar}");
  i2.tag ("baz");
  t.ok (i2.tags () == std::set <std::string> {"foo", "bar", "baz"}, "Interval(tag=foo,bar,baz) -> {foo,bar,baz}");
  i2.untag ("foo");
  t.ok (i2.tags () == std::set <std::string> {"bar", "baz"}, "Interval(tag=bar,baz) -> {bar,baz}");

  // std::string serialize () const;
  Interval i3;
  t.is (i3.serialize (), "inc", "Interval.serialize -> 'inc'");
  i3.setAnnotation ("this is an annotation");
  t.is (i3.serialize (), "inc # # \"this is an annotation\"", "Interval.serialize -> 'inc # # \"this is an annotation\"");
  i3.setAnnotation ("");
  i3.tag ("foo");
  t.is (i3.serialize (), "inc # foo", "Interval.serialize -> 'inc # foo'");
  i3.setAnnotation ("this is an annotation");
  t.is (i3.serialize (), "inc # foo # \"this is an annotation\"", "Interval.serialize -> 'inc # foo # \"this is an annotation\"");
  i3.setAnnotation ("");
  i3.tag ("bar");
  t.is (i3.serialize (), "inc # bar foo", "Interval.serialize -> 'inc # bar foo'");
  i3.start = Datetime(1);
  t.is (i3.serialize (), "inc 19700101T000001Z # bar foo", "Interval.serialize -> 'inc 19700101T000001Z # bar foo'");
  i3.setAnnotation ("this is an annotation");
  t.is (i3.serialize (), "inc 19700101T000001Z # bar foo # \"this is an annotation\"", "Interval.serialize -> 'inc 19700101T000001Z # bar foo # \"this is an annotation\"");
  i3.setAnnotation ("");
  i3.end = Datetime(2);
  t.is (i3.serialize (), "inc 19700101T000001Z - 19700101T000002Z # bar foo", "Interval.serialize -> 'inc 19700101T000001Z - 19700101T000002Z # bar foo'");
  i3.tag ("Trans-Europe Express");
  t.is (i3.serialize (), "inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo", "Interval.serialize -> 'inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo'");
  i3.setAnnotation ("this is an annotation");
  t.is (i3.serialize (), "inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo # \"this is an annotation\"", "Interval.serialize -> 'inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo' # \"this is an annotation\"");

  // Round-trip parsing.
  Interval i4;
  i4 = IntervalFactory::fromSerialization ("inc");
  t.is (i4.serialize (), "inc",
             "Round-trip 'inc'");

  Interval i5;
  i5 = IntervalFactory::fromSerialization ("inc # foo");
  t.is (i5.serialize (), "inc # foo",
             "Round-trip 'inc # foo'");

  Interval i6;
  i6 = IntervalFactory::fromSerialization ("inc # bar foo");
  t.is (i6.serialize (), "inc # bar foo",
             "Round-trip 'inc # bar foo'");

  Interval i7;
  i7 = IntervalFactory::fromSerialization ("inc 19700101T000001Z");
  t.is (i7.serialize (), "inc 19700101T000001Z",
             "Round-trip 'inc 19700101T000001Z'");

  Interval i8;
  i8 = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z");
  t.is (i8.serialize (), "inc 19700101T000001Z - 19700101T000002Z",
             "Round-trip 'inc 19700101T000001Z - 19700101T000002Z'");

  Interval i9;
  i9 = IntervalFactory::fromSerialization ("inc 19700101T000001Z # bar foo");
  t.is (i9.serialize (), "inc 19700101T000001Z # bar foo",
             "Round-trip 'inc 19700101T000001Z # bar foo'");

  Interval i10;
  i10 = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z # bar foo");
  t.is (i10.serialize (), "inc 19700101T000001Z - 19700101T000002Z # bar foo",
              "Round-trip 'inc 19700101T000001Z - 19700101T000002Z # bar foo'");

  Interval i11;
  i11 = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo");
  t.is (i11.serialize (), "inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo",
              "Round-trip 'inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo'");

  // std::string json () const;
  Interval i12;
  i12 = IntervalFactory::fromSerialization ("inc");
  t.is (i12.json (), "{}",
               "JSON '{}'");
  t.is (IntervalFactory::fromJson (i12.json ()).serialize (),
                                            "inc",
                                "Round-trip 'inc'");

  Interval i12a;
  i12a = IntervalFactory::fromSerialization ("inc # # \"this is an annotation\"");
  t.is (i12a.json (), "{\"annotation\":\"this is an annotation\"}",
                "JSON '{\"annotation\":\"this is an annotation\"}'");
  t.is (IntervalFactory::fromJson (i12a.json ()).serialize (),
                                             "inc # # \"this is an annotation\"",
                                 "Round-trip 'inc # # \"this is an annotation\"'");

  Interval i13;
  i13 = IntervalFactory::fromSerialization ("inc # foo");
  t.is (i13.json (), "{\"tags\":[\"foo\"]}",
               "JSON '{\"tags\":[\"foo\"]}'");
  t.is (IntervalFactory::fromJson (i13.json ()).serialize (),
                                            "inc # foo",
                                "Round-trip 'inc # foo'");

  Interval i13a;
  i13a = IntervalFactory::fromSerialization ("inc # foo # \"this is an annotation\"");
  t.is (i13a.json (), "{\"tags\":[\"foo\"],\"annotation\":\"this is an annotation\"}",
                "JSON '{\"tags\":[\"foo\"],\"annotation\":\"this is an annotation\"}'");
  t.is (IntervalFactory::fromJson (i13a.json ()).serialize (),
                                            "inc # foo # \"this is an annotation\"",
                                "Round-trip 'inc # foo # \"this is an annotation\"'");

  Interval i14;
  i14 = IntervalFactory::fromSerialization ("inc # bar foo");
  t.is (i14.json (), "{\"tags\":[\"bar\",\"foo\"]}",
               "JSON '{\"tags\":[\"bar\",\"foo\"]}'");
  t.is (IntervalFactory::fromJson (i14.json ()).serialize (),
                                            "inc # bar foo",
                                "Round-trip 'inc # bar foo'");

  Interval i14a;
  i14a = IntervalFactory::fromSerialization ("inc # bar foo # \"this is an annotation\"");
  t.is (i14a.json (), "{\"tags\":[\"bar\",\"foo\"],\"annotation\":\"this is an annotation\"}",
                "JSON '{\"tags\":[\"bar\",\"foo\"],\"annotation\":\"this is an annotation\"}'");
  t.is (IntervalFactory::fromJson (i14a.json ()).serialize (),
                                            "inc # bar foo # \"this is an annotation\"",
                                "Round-trip 'inc # bar foo # \"this is an annotation\"'");

  Interval i15;
  i15 = IntervalFactory::fromSerialization ("inc 19700101T000001Z");
  t.is (i15.json (), "{\"start\":\"19700101T000001Z\"}",
               "JSON '{\"start\":\"19700101T000001Z\"}'");
  t.is (IntervalFactory::fromJson (i15.json ()).serialize (),
                                            "inc 19700101T000001Z",
                                "Round-trip 'inc 19700101T000001Z'");

  Interval i15a;
  i15a = IntervalFactory::fromSerialization ("inc 19700101T000001Z # # \"this is an annotation\"");
  t.is (i15a.json (), "{\"start\":\"19700101T000001Z\",\"annotation\":\"this is an annotation\"}",
                "JSON '{\"start\":\"19700101T000001Z\",\"annotation\":\"this is an annotation\"}'");
  t.is (IntervalFactory::fromJson (i15a.json ()).serialize (),
                                             "inc 19700101T000001Z # # \"this is an annotation\"",
                                 "Round-trip 'inc 19700101T000001Z # # \"this is an annotation\"'");

  Interval i16;
  i16 = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z");
  t.is (i16.json (), "{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\"}",
               "JSON '{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\"}'");
  t.is (IntervalFactory::fromJson (i16.json ()).serialize (),
                                            "inc 19700101T000001Z - 19700101T000002Z",
                                "Round-trip 'inc 19700101T000001Z - 19700101T000002Z'");

  Interval i16a;
  i16a = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z # # \"this is an annotation\"");
  t.is (i16a.json (), "{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"annotation\":\"this is an annotation\"}",
                "JSON '{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"annotation\":\"this is an annotation\"}'");
  t.is (IntervalFactory::fromJson (i16a.json ()).serialize (),
                                             "inc 19700101T000001Z - 19700101T000002Z # # \"this is an annotation\"",
                                 "Round-trip 'inc 19700101T000001Z - 19700101T000002Z # # \"this is an annotation\"'");

  Interval i17;
  i17 = IntervalFactory::fromSerialization ("inc 19700101T000001Z # bar foo");
  t.is (i17.json (), "{\"start\":\"19700101T000001Z\",\"tags\":[\"bar\",\"foo\"]}",
               "JSON '{\"start\":\"19700101T000001Z\",\"tags\":[\"bar\",\"foo\"]}'");
  t.is (IntervalFactory::fromJson (i17.json ()).serialize (),
                                            "inc 19700101T000001Z # bar foo",
                                "Round-trip 'inc 19700101T000001Z # bar foo'");

  Interval i18;
  i18 = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z # bar foo");
  t.is (i18.json (), "{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"tags\":[\"bar\",\"foo\"]}",
               "JSON '{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"tags\":[\"bar\",\"foo\"]}'");
  t.is (IntervalFactory::fromJson (i18.json ()).serialize (),
                                            "inc 19700101T000001Z - 19700101T000002Z # bar foo",
                                "Round-trip 'inc 19700101T000001Z - 19700101T000002Z # bar foo'");

  Interval i18a;
  i18a = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z # bar foo # \"this is an annotation\"");
  t.is (i18a.json (), "{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"tags\":[\"bar\",\"foo\"],\"annotation\":\"this is an annotation\"}",
               "JSON '{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"tags\":[\"bar\",\"foo\"],\"annotation\":\"this is an annotation\"}'");
  t.is (IntervalFactory::fromJson (i18a.json ()).serialize (),
                                            "inc 19700101T000001Z - 19700101T000002Z # bar foo # \"this is an annotation\"",
                                "Round-trip 'inc 19700101T000001Z - 19700101T000002Z # bar foo # \"this is an annotation\"'");

  Interval i19;
  i19 = IntervalFactory::fromSerialization ("inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo");
  t.is (i19.json (), "{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"tags\":[\"Trans-Europe Express\",\"bar\",\"foo\"]}",
               "JSON '{\"start\":\"19700101T000001Z\",\"end\":\"19700101T000002Z\",\"tags\":[\"Trans-Europe Express\",\"bar\",\"foo\"]}'");
  t.is (IntervalFactory::fromJson (i19.json ()).serialize (),
                                            "inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo",
                                "Round-trip 'inc 19700101T000001Z - 19700101T000002Z # \"Trans-Europe Express\" bar foo");

  Interval i20;
  i20.tag ("foo");
  t.ok    (i20.hasTag ("foo"), "hasTag positive");
  t.notok (i20.hasTag ("bar"), "hasTag negative");

  Interval i21;
  i21.tag ("one-two");
  i21.tag ("three");
  t.is (i21.json (), "{\"tags\":[\"one-two\",\"three\"]}",
               "JSON '{\"tags\":[\"one-two\",\"three\"]}'");

  // make sure that underscores are escaped
  Interval i22;
  i22.tag ("foo_bar");
  t.is (i22.serialize (), "inc # \"foo_bar\"", "Interval().serialize -> 'inc # \"foo_bar\"'");



  return 0;
}

////////////////////////////////////////////////////////////////////////////////
