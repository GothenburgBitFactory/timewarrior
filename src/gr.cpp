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
#include <FS.h>
#include <Grammar.h>
#include <LR0.h>
#include <iostream>
#include <string>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
void usage ()
{
  std::cout << "\n"
            << "Usage: gr [options] <grammar> [<args>]\n"
            << "\n"
            << "Options are:\n"
            << "  -h/--help       Command usage\n"
            << "\n";
  exit (-1);
}

////////////////////////////////////////////////////////////////////////////////
int main (int argc, char** argv)
{
  // Process command line arguments
  std::string grammarFile = "";
  std::string commandLine = "";
/*
  std::vector <std::string> args;
*/

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
    {
           if (!strcmp (argv[i], "-h"))         usage ();
      else if (!strcmp (argv[i], "--help"))     usage ();
      else
      {
        std::cout << "Unrecognized option '" << argv[i] << "'" << std::endl;
        usage ();
      }
    }
    else if (grammarFile == "")
    {
      grammarFile = argv[i];
    }
    else
    {
      if (commandLine != "")
        commandLine += " ";

      commandLine += "'" + std::string (argv[i]) + "'";
    }
  }

  // Display usage for incorrect command line.
  if (grammarFile == "")
    usage ();

  try
  {
    File file (grammarFile);
    Grammar grammar;
    grammar.loadFromFile (file);
    std::cout << grammar.dump ();

    // TODO Test commandLine against grammar.
    if (commandLine != "")
    {
      LR0 lr0;
      lr0.createParseTables (grammar);
      std::cout << lr0.dump ();

      // TODO Run the parser.
    }
  }

  catch (const std::string& error)
  {
    std::cout << error << "\n";
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
