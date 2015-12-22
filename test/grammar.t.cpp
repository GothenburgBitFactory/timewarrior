////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015, Paul Beckingham, Federico Hernandez.
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
#include <Grammar.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (4);

  // Test loading from a missing file.
  Grammar g;
  File missing ("/tmp/does/not/exist");
  t.ok (! missing.exists (), "Input file does not exist");
  try
  {
    g.loadFromFile (missing);
    t.fail ("Grammar::loadFromFile accepted a missing file");
  }
  catch (std::string s)
  {
    t.diag (s);
    t.pass ("Grammar::loadFromFile rejected a missing file");
  }

  // Test error on parsing an empty grammar.
  try
  {
    g.loadFromString ("");
    t.fail ("Grammar::loadFromFile accepted an empty grammar");
  }
  catch (std::string s)
  {
    t.diag (s);
    t.pass ("Grammar::loadFromFile rejected an empty grammar");
  }

  // Test error on non-empty but trivial grammar.
  try
  {
    g.loadFromString ("# Comment\n# comment\n\n\n\n");
    t.fail ("Grammar::loadFromFile accepted an empty grammar");
  }
  catch (std::string s)
  {
    t.diag (s);
    t.pass ("Grammar::loadFromFile rejected an empty grammar");
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

