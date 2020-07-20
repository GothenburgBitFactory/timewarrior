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
#include <Datafile.h>
#include <Interval.h>

#include <TempDir.h>

int main ()
{
  UnitTest t (2);
  TempDir tempDir;

  try
  {
    Datafile df;
    Interval interval {Datetime ("2020-06-01T01:00:00"), Datetime ("2020-06-01T02:00:00")};

    df.initialize ("2020-06.data");
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
  return 0;
}

