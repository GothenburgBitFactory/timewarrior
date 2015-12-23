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
#include <LR0.h>
#include <iostream>
#include <new>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
int main (int argc, const char** argv)
{
  int status = 0;

  // Lightweight version checking that doesn't require initialization or any I/O.
  if (argc == 2 && ! strcmp (argv[1], "--version"))
  {
    std::cout << VERSION << "\n";
  }
  else
  {
    try
    {
      // TODO Load configuration ~/.timewrc || $TIMEWRC.

      // TODO Load CLI grammar.
      // TODO Load from string, else file on config override.
      // TODO Migrate from loading a grammar from file, to a default string.
      File cliFile ("./cli.grammar");
      Grammar cliGrammar;
      cliGrammar.loadFromFile (cliFile);
      std::cout << cliGrammar.dump ();

      // Instantiate the parser.
      LR0 cliParser;
      cliParser.createParseTables (cliGrammar);
      std::cout << cliParser.dump ();

      // TODO Parse CLI.

      // TODO Load rule grammar.
      File ruleFile ("./rule.grammar");
      Grammar ruleGrammar;
      ruleGrammar.loadFromFile (ruleFile);
      std::cout << ruleGrammar.dump ();

      // Instantiate the parser.
      LR0 ruleParser;
      ruleParser.createParseTables (ruleGrammar);
      std::cout << ruleParser.dump ();

      // TODO Load rules.
      // TODO Parse rules.

      // TODO Dispatch to commands.
    }

    catch (const std::string& error)
    {
      std::cerr << error << "\n";
      status = -1;
    }

    catch (std::bad_alloc& error)
    {
      std::cerr << "Error: Memory allocation failed: " << error.what () << "\n";
      status = -3;
    }

    catch (...)
    {
      std::cerr << "Error: Unknown problem, please report.\n";
      status = -2;
    }
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////
