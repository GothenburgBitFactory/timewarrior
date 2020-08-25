////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020, Shaun Ruffell, Thomas Lauf.
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

#include <test.h>
#include <vector>
#include <algorithm>
#include <Datafile.h>
#include <Interval.h>

#include <TempDir.h>

bool test_datafile_stays_sorted (UnitTest& t)
{
  TempDir tmp; 

  Datafile df {"2020-06.data"};

  const std::vector <std::string>& lines = df.allLines ();

  df.addInterval ({Datetime ("2020-06-02T01:00:00"), Datetime ("2020-06-02T02:00:00")});
  t.ok (std::is_sorted (lines.begin (), lines.end ()), "Datafile stays sorted 1");
  df.addInterval ({Datetime ("2020-06-01T01:00:00"), Datetime ("2020-06-01T02:00:00")});
  t.ok (std::is_sorted (lines.begin (), lines.end ()), "Datafile stays sorted 2");
  df.addInterval ({Datetime ("2020-06-04T01:00:00"), Datetime ("2020-06-04T02:00:00")});
  t.ok (std::is_sorted (lines.begin (), lines.end ()), "Datafile stays sorted 3");
  df.addInterval ({Datetime ("2020-06-03T01:00:00"), Datetime ("2020-06-03T02:00:00")});
  t.ok (std::is_sorted (lines.begin (), lines.end ()), "Datafile stays sorted 4");

  return true;
}

int main ()
{
  UnitTest t (6);
  TempDir tempDir;

  try
  {
    Interval interval {Datetime ("2020-06-01T01:00:00"), Datetime ("2020-06-01T02:00:00")};

    Datafile df {"2020-06.data"};
    df.addInterval (interval);

    Interval modified {interval};
    modified.tag ("foo");

    std::string message = "Datafile::deleteInterval throws on error";
    try { df.deleteInterval (modified); t.fail (message); }
    catch (...) { t.pass (message); }

    message = "Datafile::deleteInterval does not throw on success";
    try { df.deleteInterval (interval); t.pass (message); }
    catch (...) { t.fail (message); }
  }
  catch (...)
  {
    t.fail ("Uncaught exception");
  }

  test_datafile_stays_sorted (t);

  return 0;
}

