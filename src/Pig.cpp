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
#include <Pig.h>
#include <Lexer.h>
#include <utf8.h>
#include <sstream>
#include <cinttypes>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////////
Pig::Pig (const std::string& text)
: _text (text)
, _cursor (0)
, _saved (0)
{
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::skipN (const int quantity)
{
  auto save = _cursor;

  auto count = 0;
  while (count++ < quantity)
  {
    if (! utf8_next_char (_text, _cursor))
    {
      _cursor = save;
      return false;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::skipWS ()
{
  auto save = _cursor;

  int c;
  auto prev = _cursor;
  while ((c = utf8_next_char (_text, _cursor)))
  {
    if (! Lexer::isWhitespace (c))
    {
      _cursor = prev;
      break;
    }
    prev = _cursor;
  }

  return _cursor > save;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::skipLiteral (const std::string& literal)
{
  if (_text.find (literal, _cursor) == _cursor)
  {
    _cursor += literal.length ();
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::getUntilWS (std::string& result)
{
  auto save = _cursor;

  int c;
  auto prev = _cursor;
  while ((c = utf8_next_char (_text, _cursor)))
  {
    if (eos ())
    {
      result = _text.substr (save, _cursor - save);
      return true;
    }

    else if (Lexer::isWhitespace (c))
    {
      _cursor = prev;
      result = _text.substr (save, _cursor - save);
      return true;
    }

    prev = _cursor;
  }

  return _cursor > save;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::getDigit (int& result)
{
  int c = _text[_cursor];
  if (c &&
      Lexer::isDigit (c))
  {
    result = c - '0';
    ++_cursor;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::getDigits (int& result)
{
  auto save = _cursor;

  int c;
  auto prev = _cursor;
  while ((c = utf8_next_char (_text, _cursor)))
  {
    if (! Lexer::isDigit (c))
    {
      _cursor = prev;
      break;
    }
    prev = _cursor;
  }

  if (_cursor > save)
  {
    result = std::strtoimax (_text.substr (save, _cursor - save).c_str (), NULL, 10);
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// number:
//   int frac? exp?
//
// int:
//   (-|+)? digit+
//
// frac:
//   . digit+
//
// exp:
//   e digit+
//
// e:
//   e|E (+|-)?
//
bool Pig::getNumber (std::string& result)
{
  auto i = _cursor;

  // [+-]?
  if (_text[i] &&
      (_text[i] == '-' ||
       _text[i] == '+'))
    ++i;

  // digit+
  if (_text[i] &&
      Lexer::isDigit (_text[i]))
  {
    ++i;

    while (_text[i] && Lexer::isDigit (_text[i]))
      ++i;

    // ( . digit+ )?
    if (_text[i] && _text[i] == '.')
    {
      ++i;

      while (_text[i] && Lexer::isDigit (_text[i]))
        ++i;
    }

    // ( [eE] [+-]? digit+ )?
    if (_text[i] &&
        (_text[i] == 'e' ||
         _text[i] == 'E'))
    {
      ++i;

      if (_text[i] &&
          (_text[i] == '+' ||
           _text[i] == '-'))
        ++i;

      if (_text[i] && Lexer::isDigit (_text[i]))
      {
        ++i;

        while (_text[i] && Lexer::isDigit (_text[i]))
          ++i;

        result = _text.substr (_cursor, i - _cursor);
        _cursor = i;
        return true;
      }

      return false;
    }

    result = _text.substr (_cursor, i - _cursor);
    _cursor = i;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::getNumber (double& result)
{
  std::string s;
  if (getNumber (s))
  {
    result = std::strtof (s.c_str (), NULL);
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::getRemainder (std::string& result)
{
  if (_text[_cursor])
  {
    result = _text.substr (_cursor);
    _cursor += result.length ();
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Pig::eos () const
{
  return _text[_cursor] == '\0';
}

////////////////////////////////////////////////////////////////////////////////
// Peeks ahead - does not move cursor.
int Pig::peek () const
{
  return _text[_cursor];
}

////////////////////////////////////////////////////////////////////////////////
// Peeks ahead - does not move cursor.
std::string Pig::peek (const int quantity) const
{
  if (_text[_cursor] &&
      _cursor + quantity <= _text.length ())
    return _text.substr (_cursor, quantity);

  return "";
}

////////////////////////////////////////////////////////////////////////////////
std::string::size_type Pig::cursor ()
{
  return _cursor;
}

////////////////////////////////////////////////////////////////////////////////
// Note: never called internally, otherwise the client cannot rely on iṫ.
std::string::size_type Pig::save ()
{
  return _saved = _cursor;
}

////////////////////////////////////////////////////////////////////////////////
// Note: never called internally, otherwise the client cannot rely on iṫ.
std::string::size_type Pig::restore ()
{
  return _cursor = _saved;
}

////////////////////////////////////////////////////////////////////////////////
std::string Pig::dump () const
{
  std::stringstream out;
  out << "≪" << _text << "≫"
      << " l" << _text.length ()
      << " c" << _cursor;

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
