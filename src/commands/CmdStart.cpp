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
#include <commands.h>
#include <Interval.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdStart (
  const std::vector <std::string>& args,
  Rules& rules,
  Database& database,
  Log& log)
{
  // Load the most recent interval.
  auto latest = database.getLatestInterval ();

  // If the latest interval is open, close it.
  if (  latest.isStarted () &&
      ! latest.isEnded ())
  {
    // Stop it.
    latest.end (Datetime ());

    // Update database.
    database.modifyInterval (latest);
    log.write ("debug", std::string ("Stopped tracking: ") + latest.serialize ());

    // User feedback.
    std::cout << latest.summarize ();
  }

  // Create a new interval.
  Interval now;
  now.start (Datetime ());

  // Apply tags.
  for (auto& tag : std::vector <std::string> (args.begin () + 2, args.end ()))
    now.tag (tag);

  // Update database.
  database.addInterval (now);
  log.write ("debug", std::string ("Started tracking: ") + now.serialize ());

  // User feedback.
  std::cout << now.summarize ();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
