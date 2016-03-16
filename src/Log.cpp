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
#include <Log.h>
#include <format.h>

////////////////////////////////////////////////////////////////////////////////
void Log::file (const std::string& name)
{
  _name = name;
  _file = File (_name);
}

////////////////////////////////////////////////////////////////////////////////
void Log::write (const std::string& category, const std::string& line)
{
  if (line != "")
  {
    if (line == _prior)
    {
      ++_repetition;
    }
    else
    {
      _prior = line;

      // Catch up by writing out the backlog.
      if (_name != "" && _backlog.size ())
      {
        for (const auto& line : _backlog)
          _file.append (line); 

        _backlog.clear ();
      }

      if (_repetition)
      {
        std::string message = Datetime ().toISO ()
                            + " " + PACKAGE_VERSION
                            + " " + category
                            + " " + format ("(Repeated {1} times)", _repetition)
                            + "\n";

        if (_name != "")
          _file.append (message);
        else
          _backlog.push_back (message);

        _repetition = 0;
      }

      std::string message = Datetime ().toISO ()
                          + " " + PACKAGE_VERSION
                          + " " + category
                          + " " + line
                          + "\n";

      if (_name != "")
        _file.append (message);
      else
        _backlog.push_back (message);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
