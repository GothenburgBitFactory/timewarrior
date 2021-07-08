////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, 2018 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_RULES
#define INCLUDED_RULES

#include <Lexer.h>
#include <map>
#include <vector>
#include <string>
#include <Database.h>
#include <Journal.h>

class Rules
{
public:
  Rules ();
  Rules (const Rules&) = delete;
  Rules (Rules&&) = default;
  Rules& operator= (const Rules&) = delete;
  Rules& operator= (Rules&&) = default;

  void load (const std::string&, int next = 1);
  std::string file () const;

  bool        has        (const std::string&) const;
  std::string get (const std::string &key, const std::string &defaultValue = "") const;
  int         getInteger (const std::string&, int defaultValue = 0) const;
  double      getReal    (const std::string&) const;
  bool        getBoolean (const std::string&) const;

  void set (const std::string&, const int);
  void set (const std::string&, const double);
  void set (const std::string&, const std::string&);

  std::vector <std::string> all (const std::string& stem = "") const;
  bool isRuleType (const std::string&) const;

  std::string dump () const;

  static bool setConfigVariable (Journal& journal,
                                 const Rules& rules,
                                 std::string name,
                                 std::string value,
                                 bool confirmation /* = false */);
  static int unsetConfigVariable (Journal& journal,
                                  const Rules& rules,
                                  std::string name,
                                  bool confirmation /* = false */);

private:
  void parse               (const std::string&, int next = 1);
  void parseRule           (const std::string&);
  void parseRuleSettings   (const std::vector <std::string>&);

  unsigned int getIndentation (const std::string&);
  std::vector <std::string> tokenizeLine (const std::string&);
  std::string parseGroup   (const std::vector <std::string>&);

private:
  std::string                         _original_file {};
  std::map <std::string, std::string> _settings      {};
  std::vector <std::string>           _rule_types    {"tags", "reports", "theme", "holidays", "exclusions"};

};

#endif
