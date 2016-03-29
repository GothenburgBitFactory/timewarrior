////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013 - 2016, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_LEXER
#define INCLUDED_LEXER

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <cstddef>

class Lexer
{
public:
  // These are overridable.
  static std::string dateFormat;

  enum class Type { number, hex,
                    string,
                    url,
                    path,
                    pattern,
                    op,
                    word,
                    date, duration };

  explicit Lexer (const std::string&);
  bool token (std::string&, Lexer::Type&);
  static std::string typeToString (Lexer::Type);

  // Static helpers.
  static std::vector <std::tuple <std::string, Lexer::Type>> tokenize (const std::string&);
  static const std::string typeName          (const Lexer::Type&);
  static bool isSingleCharOperator           (int);
  static bool isDoubleCharOperator           (int, int, int);
  static bool isTripleCharOperator           (int, int, int, int);
  static bool isBoundary                     (int, int);
  static bool isHardBoundary                 (int, int);
  static bool isPunctuation                  (int);
  static bool readWord                       (const std::string&, const std::string&, std::string::size_type&, std::string&);
  static bool readWord                       (const std::string&, std::string::size_type&, std::string&);
  static int hexToInt                        (int);
  static int hexToInt                        (int, int);
  static int hexToInt                        (int, int, int, int);
  static std::string trimLeft                (const std::string& in, const std::string& t = " ");
  static std::string trimRight               (const std::string& in, const std::string& t = " ");
  static std::string trim                    (const std::string& in, const std::string& t = " ");
  static std::string dequote                 (const std::string&, const std::string& quotes = "'\"");

  // Stream Classifiers.
  bool isEOS          () const;
  bool isString       (std::string&, Lexer::Type&, const std::string&);
  bool isDate         (std::string&, Lexer::Type&);
  bool isDuration     (std::string&, Lexer::Type&);
  bool isNumber       (std::string&, Lexer::Type&);
  bool isInteger      (std::string&, Lexer::Type&);
  bool isHexNumber    (std::string&, Lexer::Type&);
  bool isURL          (std::string&, Lexer::Type&);
  bool isPath         (std::string&, Lexer::Type&);
  bool isPattern      (std::string&, Lexer::Type&);
  bool isOperator     (std::string&, Lexer::Type&);
  bool isWord         (std::string&, Lexer::Type&);

private:
  std::string _text   {};
  std::size_t _cursor {0};
  std::size_t _eos    {0};
};

#endif
