////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2017, Paul Beckingham, Federico Hernandez.
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
#include <shared.h>
#include <timew.h>
#include <algorithm>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdShow (Rules& rules)
{
  // Obtain and sort the names. Sorting is critical.
  auto names = rules.all ();
  std::sort (names.begin (), names.end ());

  std::vector <std::string> previous {};
  for (auto& name : rules.all ())
  {
    if (name.substr (0, 5) == "temp.")
      continue;

    auto parts = split (name, '.');
    for (unsigned int i = 0; i < parts.size (); ++i)
    {
      // The last part is special.
      if (i == parts.size () - 1)
      {
        if (previous.size () > 1 &&
            parts.size () == 1)
          std::cout << '\n';

        std::cout << std::string (2 * (parts.size () - 1), ' ')
                  << parts[i]
                  << " = "
                  << rules.get (name)
                  << "\n";
      }
      else
      {
        if (previous.size () <= i ||
            previous[i] != parts[i])
        {
          if (i == 0)
          {
            std::cout << '\n';
            if (rules.isRuleType (parts[0]))
              std::cout << "define ";
          }

          std::cout << std::string (2 * i, ' ')
                    << parts[i]
                    << ":\n";
        }
      }
    }

    previous = parts;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
