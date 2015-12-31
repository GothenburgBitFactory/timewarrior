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
#include <Grammar.h>
#include <test.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
void testBadGrammarFile (UnitTest& t, File& file)
{
  try
  {
    Grammar g;
    g.loadFromFile (file);
    t.fail ("Grammar::loadFromFile accepted a bad file");
  }
  catch (std::string error)
  {
    t.diag (error);
    t.pass ("Grammar::loadFromFile rejected a bad file");
  }
}

////////////////////////////////////////////////////////////////////////////////
void testBadGrammar (UnitTest& t, const std::string& bnf)
{
  try
  {
    Grammar g;
    g.loadFromString (bnf);
    t.fail ("Grammar::loadFromFile accepted a bad grammar");
  }
  catch (std::string error)
  {
    t.diag (error);
    t.pass ("Grammar::loadFromFile rejected a bad grammar");
  }
}

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (20);

  // Test loading from a missing file.
  File missing ("/tmp/does/not/exist");
  t.notok (missing.exists (), "Grammar file is recognized as missing");
  testBadGrammarFile (t, missing);

  // Test error on parsing bad grammar.
  testBadGrammar (t, "");
  testBadGrammar (t, "# Comment\n"
                     "# Comment\n"
                     "\n"
                     "\n"
                     "\n");

  // Unreferenced rule 'two'.
  testBadGrammar (t, "one: \"foo\"\n"
                     "\n"
                     "two: \"bar\"\n");

  // Undefined rule 'three'.
  testBadGrammar (t, "one: three\n"
                     "\n"
                     "two: \"foo\"\n");

  // Test metadata access.
  Grammar g;
  g.loadFromString ("one: two\n"
                    "\n"
                    "two: three \"literal\"\n"
                    "     /regex/\n"
                    "\n"
                    "three: \"literal2\"\n");
  auto start     = g.start ();
  auto rules     = g.rules ();
  auto terminals = g.terminals ();
  auto augmented = g.augmented ();
  t.is (start, "one",                                                                         "Located start rule");

  t.is ((int)rules.size (), 3,                                                                "Found three rules");
  t.ok (std::find (rules.begin (), rules.end (), "one")                  != rules.end (),     "Found rule 'one'");
  t.ok (std::find (rules.begin (), rules.end (), "two")                  != rules.end (),     "Found rule 'two'");
  t.ok (std::find (rules.begin (), rules.end (), "three")                != rules.end (),     "Found rule 'three'");

  t.ok (std::find (terminals.begin (), terminals.end (), "\"literal\"")  != terminals.end (), "Found terminal '\"literal\"'");
  t.ok (std::find (terminals.begin (), terminals.end (), "/regex/")      != terminals.end (), "Found terminal '/regex/'");
  t.ok (std::find (terminals.begin (), terminals.end (), "\"literal2\"") != terminals.end (), "Found terminal '\"literal2\"'");

  t.is ((int)augmented.size (), 5,                                                       "augmented: 5 items");
  t.ok (augmented[0] == std::vector <std::string>{"S", "-->", "one"},                    "augmented[0]: S --> one");
  t.ok (augmented[1] == std::vector <std::string>{"one", "-->", "two"},                  "augmented[1]: one --> two");
  t.ok (augmented[2] == std::vector <std::string>{"three", "-->", "\"literal2\""},       "augmented[2]: three --> \"literal2\"");
  t.ok (augmented[3] == std::vector <std::string>{"two", "-->", "three", "\"literal\""}, "augmented[3]: two --> three \"literal\"");
  t.ok (augmented[4] == std::vector <std::string>{"two", "-->", "/regex/"},              "augmented[4]: two --> /regex/");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

