////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2019, 2022 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <test.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
UnitTest::UnitTest ()
: _planned (0)
, _counter (0)
, _passed (0)
, _failed (0)
, _skipped (0)
{
}

///////////////////////////////////////////////////////////////////////////////
UnitTest::UnitTest (int planned)
: _planned (planned)
, _counter (0)
, _passed (0)
, _failed (0)
, _skipped (0)
{
  std::cout << "1.." << _planned << '\n';
}

///////////////////////////////////////////////////////////////////////////////
UnitTest::~UnitTest ()
{
  float percentPassed = 0.0;
  if (_planned > 0)
  {
    percentPassed = (100.0 * _passed) / std::max (_planned, _passed + _failed + _skipped);
  }

  if (_counter < _planned)
  {
    std::cout << "# Only "
              << _counter
              << " tests, out of a planned "
              << _planned
              << " were run.\n";
    _skipped += _planned - _counter;
  }

  else if (_counter > _planned)
  {
    std::cout << "# "
              << _counter
              << " tests were run, but only "
              << _planned
              << " were planned.\n";
  }

  std::cout << "# "
            << _passed
            << " passed, "
            << _failed
            << " failed, "
            << _skipped
            << " skipped. "
            << std::setprecision (3) << percentPassed
            << "% passed.\n";
  exit (_failed > 0);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::plan (int planned)
{
  _planned = planned;
  _counter = 0;
  _passed = 0;
  _failed = 0;
  _skipped = 0;

  std::cout << "1.." << _planned << '\n';
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::planMore (int extra)
{
  _planned += extra;
  std::cout << "1.." << _planned << '\n';
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::ok (bool expression, const std::string& name)
{
  ++_counter;

  if (expression)
  {
    report_success (name);
  }
  else
  {
    report_failure (name);
  }
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::notok (bool expression, const std::string& name)
{
  ok (! expression, name);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (bool actual, bool expected, const std::string& name)
{
  _is <bool> (actual, expected, name);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (size_t actual, size_t expected, const std::string& name)
{
  _is <size_t> (actual, expected, name);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (int actual, int expected, const std::string& name)
{
  _is <int> (actual, expected, name);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (double actual, double expected, const std::string& name)
{
  _is <double> (actual, expected, name);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (double actual, double expected, double tolerance, const std::string& name)
{
  ++_counter;

  if (fabs (actual - expected) <= tolerance)
  {
    report_success (name);
  }
  else
  {
    report_failure (create_message <double> (actual, expected, name));
  }
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (unsigned char actual, unsigned char expected, const std::string& name)
{
  _is <unsigned char> (actual, expected, name);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (
  const std::string& actual,
  const std::string& expected,
  const std::string& name)
{
  _is <const std::string&> (actual, expected, name);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::is (
  const char* actual,
  const char* expected,
  const std::string& name)
{
  ++_counter;

  if (! strcmp (actual, expected))
  {
    report_success (name);
  }
  else
  {
    report_failure (create_message <const char*> (actual, expected, name));
  }
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
void UnitTest::_is (T actual, T expected, const std::string& name)
{
  ++_counter;

  if (actual == expected)
  {
    report_success (name);
  }
  else
  {
    report_failure (create_message <T> (actual, expected, name));
  }
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::diag (const std::string& text)
{
  auto start = text.find_first_not_of (" \t\n\r\f");
  auto end   = text.find_last_not_of  (" \t\n\r\f");

  if (start != std::string::npos &&
      end   != std::string::npos)
  {
    std::cout << "# " << text.substr (start, end - start + 1) << '\n';
  }
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::pass (const std::string& text)
{
  ++_counter;
  report_success (text);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::fail (const std::string& text)
{
  ++_counter;
  report_failure (text);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::skip (const std::string& text)
{
  ++_counter;
  report_skip (text);
}

///////////////////////////////////////////////////////////////////////////////
template <typename T>
std::string UnitTest::create_message (T actual, T expected, const std::string& name)
{
  std::stringstream message;

  message << name
          << "\n# expected: '"
          << expected
          << "'"
          << "\n#      got: '"
          << actual
          << "'";

  return message.str ();
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::report_success (const std::string& message)
{
  ++_passed;
  report_status (green ("ok"), message);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::report_failure (const std::string& message)
{
  ++_failed;
  report_status (red ("not ok"), message);
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::report_skip (const std::string& message)
{
  ++_skipped;
  report_status (yellow ("ok"), message + " # skip");
}

///////////////////////////////////////////////////////////////////////////////
void UnitTest::report_status (const std::string& status, const std::string& message) const
{
  std::cout << status
            << " "
            << _counter
            << " - "
            << message
            << '\n';
}

///////////////////////////////////////////////////////////////////////////////
std::string UnitTest::red (const std::string& input) const
{
  return colored (COLOR_RED, input);
}

///////////////////////////////////////////////////////////////////////////////
std::string UnitTest::green (const std::string& input) const
{
  return colored (COLOR_GREEN, input);
}

///////////////////////////////////////////////////////////////////////////////
std::string UnitTest::yellow (const std::string& input) const
{
  return colored (COLOR_YELLOW, input);
}

///////////////////////////////////////////////////////////////////////////////
std::string UnitTest::colored (const std::string& color_code, const std::string& input) const
{
  if (isatty (fileno (stdout)))
  {
    return std::string (color_code + input + COLOR_RESET);
  }

  return input;
}

///////////////////////////////////////////////////////////////////////////////
