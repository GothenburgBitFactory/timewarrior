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
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> getKeywords (const std::vector <std::string>& args)
{
  std::vector <std::string> keywords;
  for (auto& arg : args)
    if (arg[0] == ':')
      keywords.push_back (arg);

  return keywords;
}

////////////////////////////////////////////////////////////////////////////////
// enum class ArgType { binary, command, positional, keyword };
ArgType classifyArg (const std::string& arg)
{
  if (arg.find ("timew") == arg.length () - 5 ||
      arg.find ("ti")    == arg.length () - 2)
    return ArgType::binary;

  // TODO Commands are a problem.

  if (arg[0] == ':')
    return ArgType::keyword;

  // The positional args are really just the remainder after the others are
  // excluded.
  return ArgType::positional;
}

////////////////////////////////////////////////////////////////////////////////
