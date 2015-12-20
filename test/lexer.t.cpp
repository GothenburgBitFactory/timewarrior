////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013 - 2015, Göteborg Bit Factory.
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
#include <Lexer.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <test.h>

////////////////////////////////////////////////////////////////////////////////
int main (int, char**)
{
  UnitTest t (50);

  std::vector <std::pair <std::string, Lexer::Type>> tokens;
  std::string token;
  Lexer::Type type;

  // White space detection.
  t.notok (Lexer::isWhitespace (0x0041), "U+0041 (A) ! isWhitespace");
  t.ok (Lexer::isWhitespace (0x0020), "U+0020 isWhitespace");
  t.ok (Lexer::isWhitespace (0x0009), "U+0009 isWhitespace");
  t.ok (Lexer::isWhitespace (0x000A), "U+000A isWhitespace");
  t.ok (Lexer::isWhitespace (0x000B), "U+000B isWhitespace");
  t.ok (Lexer::isWhitespace (0x000C), "U+000C isWhitespace");
  t.ok (Lexer::isWhitespace (0x000D), "U+000D isWhitespace");
  t.ok (Lexer::isWhitespace (0x0085), "U+0085 isWhitespace");
  t.ok (Lexer::isWhitespace (0x00A0), "U+00A0 isWhitespace");
  t.ok (Lexer::isWhitespace (0x1680), "U+1680 isWhitespace"); // 10
  t.ok (Lexer::isWhitespace (0x180E), "U+180E isWhitespace");
  t.ok (Lexer::isWhitespace (0x2000), "U+2000 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2001), "U+2001 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2002), "U+2002 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2003), "U+2003 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2004), "U+2004 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2005), "U+2005 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2006), "U+2006 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2007), "U+2007 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2008), "U+2008 isWhitespace"); // 20
  t.ok (Lexer::isWhitespace (0x2009), "U+2009 isWhitespace");
  t.ok (Lexer::isWhitespace (0x200A), "U+200A isWhitespace");
  t.ok (Lexer::isWhitespace (0x2028), "U+2028 isWhitespace");
  t.ok (Lexer::isWhitespace (0x2029), "U+2029 isWhitespace");
  t.ok (Lexer::isWhitespace (0x202F), "U+202F isWhitespace");
  t.ok (Lexer::isWhitespace (0x205F), "U+205F isWhitespace");
  t.ok (Lexer::isWhitespace (0x3000), "U+3000 isWhitespace");

  // Should result in no tokens.
  Lexer l0 ("");
  t.notok (l0.token (token, type), "'' --> no tokens");

  // Should result in no tokens.
  Lexer l1 ("       \t ");
  t.notok (l1.token (token, type), "'       \\t ' --> no tokens");

  // std::string Lexer::trimLeft (const std::string& in, const std::string&)
  t.is (Lexer::trimLeft (""),                     "",            "Lexer::trimLeft '' -> ''");
  t.is (Lexer::trimLeft ("   "),                  "",            "Lexer::trimLeft '   ' -> ''");
  t.is (Lexer::trimLeft ("",              " \t"), "",            "Lexer::trimLeft '' -> ''");
  t.is (Lexer::trimLeft ("xxx"),                  "xxx",         "Lexer::trimLeft 'xxx' -> 'xxx'");
  t.is (Lexer::trimLeft ("xxx",           " \t"), "xxx",         "Lexer::trimLeft 'xxx' -> 'xxx'");
  t.is (Lexer::trimLeft ("  \t xxx \t  "),        "\t xxx \t  ", "Lexer::trimLeft '  \\t xxx \\t  ' -> '\\t xxx \\t  '");
  t.is (Lexer::trimLeft ("  \t xxx \t  ", " \t"), "xxx \t  ",    "Lexer::trimLeft '  \\t xxx \\t  ' -> 'xxx \\t  '");

  // std::string Lexer::trimRight (const std::string& in, const std::string&)
  t.is (Lexer::trimRight (""),                     "",            "Lexer::trimRight '' -> ''");
  t.is (Lexer::trimRight ("   "),                  "",            "Lexer::trimRight '   ' -> ''");
  t.is (Lexer::trimRight ("",              " \t"), "",            "Lexer::trimRight '' -> ''");
  t.is (Lexer::trimRight ("xxx"),                  "xxx",         "Lexer::trimRight 'xxx' -> 'xxx'");
  t.is (Lexer::trimRight ("xxx",           " \t"), "xxx",         "Lexer::trimRight 'xxx' -> 'xxx'");
  t.is (Lexer::trimRight ("  \t xxx \t  "),        "  \t xxx \t", "Lexer::trimRight '  \\t xxx \\t  ' -> '  \\t xxx \\t'");
  t.is (Lexer::trimRight ("  \t xxx \t  ", " \t"), "  \t xxx",    "Lexer::trimRight '  \\t xxx \\t  ' -> '  \\t xxx'");

  // std::string Lexer::trim (const std::string& in, const std::string& t)
  t.is (Lexer::trim (""),                     "",          "Lexer::trim '' -> ''");
  t.is (Lexer::trim ("   "),                  "",          "Lexer::trim '   ' -> ''");
  t.is (Lexer::trim ("",              " \t"), "",          "Lexer::trim '' -> ''");
  t.is (Lexer::trim ("xxx"),                  "xxx",       "Lexer::trim 'xxx' -> 'xxx'");
  t.is (Lexer::trim ("xxx",           " \t"), "xxx",       "Lexer::trim 'xxx' -> 'xxx'");
  t.is (Lexer::trim ("  \t xxx \t  "),        "\t xxx \t", "Lexer::trim '  \\t xxx \\t  ' -> '\\t xxx \\t'");
  t.is (Lexer::trim ("  \t xxx \t  ", " \t"), "xxx",       "Lexer::trim '  \\t xxx \\t  ' -> 'xxx'");

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
