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

#include <cmake.h>
#include <Lexer.h>
#include <Datetime.h>
#include <Duration.h>
#include <algorithm>
#include <tuple>
#include <ctype.h>
#include <unicode.h>
#include <utf8.h>

static const std::string uuid_pattern = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
static const unsigned int uuid_min_length = 8;

std::string Lexer::dateFormat = "";

////////////////////////////////////////////////////////////////////////////////
Lexer::Lexer (const std::string& text)
: _text (text)
, _eos (text.size ())
{
}

////////////////////////////////////////////////////////////////////////////////
// When a Lexer object is constructed with a string, this method walks through
// the stream of low-level tokens.
bool Lexer::token (std::string& token, Lexer::Type& type)
{
  // Eat white space.
  while (unicodeWhitespace (_text[_cursor]))
    utf8_next_char (_text, _cursor);

  // Terminate at EOS.
  if (isEOS ())
    return false;

  if (isString    (token, type, "'\"") ||
      isUUID      (token, type, true)  ||
      isDate      (token, type)        ||
      isDuration  (token, type)        ||
      isURL       (token, type)        ||
      isHexNumber (token, type)        ||
      isNumber    (token, type)        ||
      isPath      (token, type)        ||
      isPattern   (token, type)        ||
      isOperator  (token, type)        ||
      isWord      (token, type))
    return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::tuple <std::string, Lexer::Type>> Lexer::tokenize (const std::string& input)
{
  std::vector <std::tuple <std::string, Lexer::Type>> tokens;

  std::string token;
  Lexer::Type type;
  Lexer lexer (input);
  while (lexer.token (token, type))
    tokens.push_back (std::make_tuple (token, type));

  return tokens;
}

////////////////////////////////////////////////////////////////////////////////
// No L10N - these are for internal purposes.
const std::string Lexer::typeName (const Lexer::Type& type)
{
  switch (type)
  {
  case Lexer::Type::uuid:         return "uuid";
  case Lexer::Type::number:       return "number";
  case Lexer::Type::hex:          return "hex";
  case Lexer::Type::string:       return "string";
  case Lexer::Type::url:          return "url";
  case Lexer::Type::path:         return "path";
  case Lexer::Type::pattern:      return "pattern";
  case Lexer::Type::op:           return "op";
  case Lexer::Type::word:         return "word";
  case Lexer::Type::date:         return "date";
  case Lexer::Type::duration:     return "duration";
  }

  return "unknown";
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::number
//   \d+
//   [ . \d+ ]
//   [ e|E [ +|- ] \d+ [ . \d+ ] ]
//   not followed by non-operator.
bool Lexer::isNumber (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  if (unicodeLatinDigit (_text[marker]))
  {
    ++marker;
    while (unicodeLatinDigit (_text[marker]))
      utf8_next_char (_text, marker);

    if (_text[marker] == '.')
    {
      ++marker;
      if (unicodeLatinDigit (_text[marker]))
      {
        ++marker;
        while (unicodeLatinDigit (_text[marker]))
          utf8_next_char (_text, marker);
      }
    }

    if (_text[marker] == 'e' ||
        _text[marker] == 'E')
    {
      ++marker;

      if (_text[marker] == '+' ||
          _text[marker] == '-')
        ++marker;

      if (unicodeLatinDigit (_text[marker]))
      {
        ++marker;
        while (unicodeLatinDigit (_text[marker]))
          utf8_next_char (_text, marker);

        if (_text[marker] == '.')
        {
          ++marker;
          if (unicodeLatinDigit (_text[marker]))
          {
            ++marker;
            while (unicodeLatinDigit (_text[marker]))
              utf8_next_char (_text, marker);
          }
        }
      }
    }

    // Lookahread: !<unicodeWhitespace> | !<isSingleCharOperator>
    // If there is an immediately consecutive character, that is not an operator, fail.
    if (_eos > marker &&
        ! unicodeWhitespace (_text[marker]) &&
        ! isSingleCharOperator (_text[marker]))
      return false;

    token = _text.substr (_cursor, marker - _cursor);
    type = Lexer::Type::number;
    _cursor = marker;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::number
//   \d+
bool Lexer::isInteger (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  if (unicodeLatinDigit (_text[marker]))
  {
    ++marker;
    while (unicodeLatinDigit (_text[marker]))
      utf8_next_char (_text, marker);

    token = _text.substr (_cursor, marker - _cursor);
    type = Lexer::Type::number;
    _cursor = marker;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Lexer::isSingleCharOperator (int c)
{
  return c == '+' ||  // Addition
         c == '-' ||  // Subtraction or unary minus = ambiguous
         c == '*' ||  // Multiplication
         c == '/' ||  // Diviѕion
         c == '(' ||  // Precedence open parenthesis
         c == ')' ||  // Precedence close parenthesis
         c == '<' ||  // Less than
         c == '>' ||  // Greater than
         c == '^' ||  // Exponent
         c == '!' ||  // Unary not
         c == '%' ||  // Modulus
         c == '=' ||  // Partial match
         c == '~';    // Pattern match
}

////////////////////////////////////////////////////////////////////////////////
bool Lexer::isDoubleCharOperator (int c0, int c1, int c2)
{
  return (c0 == '=' && c1 == '=')                        ||
         (c0 == '!' && c1 == '=')                        ||
         (c0 == '<' && c1 == '=')                        ||
         (c0 == '>' && c1 == '=')                        ||
         (c0 == 'o' && c1 == 'r' && isBoundary (c1, c2)) ||
         (c0 == '|' && c1 == '|')                        ||
         (c0 == '&' && c1 == '&')                        ||
         (c0 == '!' && c1 == '~');
}

////////////////////////////////////////////////////////////////////////////////
bool Lexer::isTripleCharOperator (int c0, int c1, int c2, int c3)
{
  return (c0 == 'a' && c1 == 'n' && c2 == 'd' && isBoundary (c2, c3)) ||
         (c0 == 'x' && c1 == 'o' && c2 == 'r' && isBoundary (c2, c3)) ||
         (c0 == '!' && c1 == '=' && c2 == '=');
}

////////////////////////////////////////////////////////////////////////////////
bool Lexer::isBoundary (int left, int right)
{
  // EOS
  if (right == '\0')                                         return true;

  // XOR
  if (unicodeLatinAlpha (left) != unicodeLatinAlpha (right)) return true;
  if (unicodeLatinDigit (left) != unicodeLatinDigit (right)) return true;
  if (unicodeWhitespace (left) != unicodeWhitespace (right)) return true;

  // OR
  if (isPunctuation (left) || isPunctuation (right))         return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Lexer::isHardBoundary (int left, int right)
{
  // EOS
  if (right == '\0')
    return true;

  // FILTER operators that don't need to be surrounded by whitespace.
  if (left == '(' ||
      left == ')' ||
      right == '(' ||
      right == ')')
    return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Lexer::isPunctuation (int c)
{
  return isprint (c)   &&
         c != ' '      &&
         c != '@'      &&
         c != '#'      &&
         c != '$'      &&
         c != '_'      &&
         ! unicodeLatinDigit (c) &&
         ! unicodeLatinAlpha (c);
}

////////////////////////////////////////////////////////////////////////////////
// Assumes that quotes is a string containing a non-trivial set of quote
// characters.
std::string Lexer::dequote (const std::string& input, const std::string& quotes)
{
  if (input.length ())
  {
    int quote = input[0];
    if (quotes.find (quote) != std::string::npos)
    {
      size_t len = input.length ();
      if (quote == input[len - 1])
        return input.substr (1, len - 2);
    }
  }

  return input;
}

////////////////////////////////////////////////////////////////////////////////
// Detects characters in an input string that indicate quotes were required, or
// escapes, to get them past the shell.
bool Lexer::wasQuoted (const std::string& input)
{
  if (input.find_first_of (" \t()<>&~") != std::string::npos)
    return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool Lexer::isEOS () const
{
  return _cursor >= _eos;
}

////////////////////////////////////////////////////////////////////////////////
// Converts '0'     -> 0
//          '9'     -> 9
//          'a'/'A' -> 10
//          'f'/'F' -> 15
int Lexer::hexToInt (int c)
{
       if (c >= '0' && c <= '9') return (c - '0');
  else if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
  else                           return (c - 'A' + 10);
}

////////////////////////////////////////////////////////////////////////////////
int Lexer::hexToInt (int c0, int c1)
{
  return (hexToInt (c0) << 4) + hexToInt (c1);
}

////////////////////////////////////////////////////////////////////////////////
int Lexer::hexToInt (int c0, int c1, int c2, int c3)
{
  return (hexToInt (c0) << 12) +
         (hexToInt (c1) << 8)  +
         (hexToInt (c2) << 4)  +
          hexToInt (c3);
}

////////////////////////////////////////////////////////////////////////////////
std::string Lexer::trimLeft (const std::string& in, const std::string& t /*= " "*/)
{
  std::string::size_type ws = in.find_first_not_of (t);
  if (ws > 0)
  {
    std::string out {in};
    return out.erase (0, ws);
  }

  return in;
}

////////////////////////////////////////////////////////////////////////////////
std::string Lexer::trimRight (const std::string& in, const std::string& t /*= " "*/)
{
  std::string out {in};
  return out.erase (in.find_last_not_of (t) + 1);
}

////////////////////////////////////////////////////////////////////////////////
std::string Lexer::trim (const std::string& in, const std::string& t /*= " "*/)
{
  return trimLeft (trimRight (in, t), t);
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::string
//   '|"
//   [ U+XXXX | \uXXXX | \" | \' | \\ | \/ | \b | \f | \n | \r | \t | . ]
//   '|"
bool Lexer::isString (std::string& token, Lexer::Type& type, const std::string& quotes)
{
  std::size_t marker = _cursor;
  if (readWord (_text, quotes, marker, token))
  {
    type = Lexer::Type::string;
    _cursor = marker;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::date
//   <Datetime> (followed by eos, WS, operator)
bool Lexer::isDate (std::string& token, Lexer::Type& type)
{
  // Try an ISO date parse.
  std::size_t i = _cursor;
  Datetime d;
  if (d.parse (_text, i, Lexer::dateFormat) &&
      (i >= _eos ||
       unicodeWhitespace (_text[i]) ||
       isSingleCharOperator (_text[i])))
  {
    type = Lexer::Type::date;
    token = _text.substr (_cursor, i - _cursor);
    _cursor = i;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::duration
//   <Duration> (followed by eos, WS, operator)
bool Lexer::isDuration (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  std::string extractedToken;
  Lexer::Type extractedType;
  if (isOperator(extractedToken, extractedType))
  {
    _cursor = marker;
    return false;
  }

  marker = _cursor;
  Duration dur;
  if (dur.parse (_text, marker) &&
      (marker >= _eos ||
       unicodeWhitespace (_text[marker]) ||
       isSingleCharOperator (_text[marker])))
  {
    type = Lexer::Type::duration;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::uuid
//   XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
//   XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXX
//   XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXX
//   XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXX
//   ...
//   XXXXXXXX-XX
//   XXXXXXXX-X
//   XXXXXXXX-
//   XXXXXXXX
//   Followed only by EOS, whitespace, or single character operator.
bool Lexer::isUUID (std::string& token, Lexer::Type& type, bool endBoundary)
{
  std::size_t marker = _cursor;

  // Greedy.
  std::size_t i = 0;
  for (; i < 36 && marker + i < _eos; i++)
  {
    if (uuid_pattern[i] == 'x')
    {
      if (! unicodeHexDigit (_text[marker + i]))
        break;
    }
    else if (uuid_pattern[i] != _text[marker + i])
      break;
  }

  if (i >= uuid_min_length              &&
      (! endBoundary                    ||
       ! _text[marker + i]              ||
       unicodeWhitespace (_text[marker + i]) ||
       isSingleCharOperator (_text[marker + i])))
  {
    token = _text.substr (_cursor, i);
    type = Lexer::Type::uuid;
    _cursor += i;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::hex
//   0xX+
bool Lexer::isHexNumber (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  if (_eos - marker >= 3 &&
      _text[marker + 0] == '0' &&
      _text[marker + 1] == 'x')
  {
    marker += 2;

    while (unicodeHexDigit (_text[marker]))
      ++marker;

    if (marker - _cursor > 2)
    {
      token = _text.substr (_cursor, marker - _cursor);
      type = Lexer::Type::hex;
      _cursor = marker;
      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::word
//   [^\s]+
bool Lexer::isWord (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  while (_text[marker]                  &&
         ! unicodeWhitespace (_text[marker]) &&
         ! isSingleCharOperator (_text[marker]))
    utf8_next_char (_text, marker);

  if (marker > _cursor)
  {
    token = _text.substr (_cursor, marker - _cursor);
    type = Lexer::Type::word;
    _cursor = marker;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::url
//   http [s] :// ...
bool Lexer::isURL (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  if (_eos - _cursor > 9 &&    // length 'https://*'
      (_text[marker + 0] == 'h' || _text[marker + 0] == 'H') &&
      (_text[marker + 1] == 't' || _text[marker + 1] == 'T') &&
      (_text[marker + 2] == 't' || _text[marker + 2] == 'T') &&
      (_text[marker + 3] == 'p' || _text[marker + 3] == 'P'))
  {
    marker += 4;
    if (_text[marker + 0] == 's' || _text[marker + 0] == 'S')
      ++marker;

    if (_text[marker + 0] == ':' &&
        _text[marker + 1] == '/' &&
        _text[marker + 2] == '/')
    {
      marker += 3;

      while (marker < _eos &&
             ! unicodeWhitespace (_text[marker]))
        utf8_next_char (_text, marker);

      token = _text.substr (_cursor, marker - _cursor);
      type = Lexer::Type::url;
      _cursor = marker;
      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::path
//   ( / <non-slash, non-whitespace> )+
bool Lexer::isPath (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;
  int slashCount = 0;

  while (1)
  {
    if (_text[marker] == '/')
    {
      ++marker;
      ++slashCount;
    }
    else
      break;

    if (_text[marker] &&
        ! unicodeWhitespace (_text[marker]) &&
        _text[marker] != '/')
    {
      utf8_next_char (_text, marker);
      while (_text[marker] &&
             ! unicodeWhitespace (_text[marker]) &&
             _text[marker] != '/')
        utf8_next_char (_text, marker);
    }
    else
      break;
  }

  if (marker > _cursor &&
      slashCount > 3)
  {
    type = Lexer::Type::path;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::pattern
//   / <unquoted-string> /  <EOS> | <unicodeWhitespace>
bool Lexer::isPattern (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  std::string word;
  if (readWord (_text, "/", _cursor, word) &&
      (isEOS () ||
       unicodeWhitespace (_text[_cursor])))
  {
    token = _text.substr (marker, _cursor - marker);
    type = Lexer::Type::pattern;
    return true;
  }

  _cursor = marker;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Lexer::Type::op
//   _hastag_ | _notag | _neg_ | _pos_ |
//   <isTripleCharOperator> |
//   <isDoubleCharOperator> |
//   <isSingleCharOperator> |
bool Lexer::isOperator (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  if (_eos - marker >= 8 && _text.substr (marker, 8) == "_hastag_")
  {
    marker += 8;
    type = Lexer::Type::op;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  else if (_eos - marker >= 7 && _text.substr (marker, 7) == "_notag_")
  {
    marker += 7;
    type = Lexer::Type::op;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  else if (_eos - marker >= 5 && _text.substr (marker, 5) == "_neg_")
  {
    marker += 5;
    type = Lexer::Type::op;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  else if (_eos - marker >= 5 && _text.substr (marker, 5) == "_pos_")
  {
    marker += 5;
    type = Lexer::Type::op;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  else if (_eos - marker >= 3 &&
      isTripleCharOperator (_text[marker], _text[marker + 1], _text[marker + 2], _text[marker + 3]))
  {
    marker += 3;
    type = Lexer::Type::op;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  else if (_eos - marker >= 2 &&
      isDoubleCharOperator (_text[marker], _text[marker + 1], _text[marker + 2]))
  {
    marker += 2;
    type = Lexer::Type::op;
    token = _text.substr (_cursor, marker - _cursor);
    _cursor = marker;
    return true;
  }

  else if (isSingleCharOperator (_text[marker]))
  {
    token = _text[marker];
    type = Lexer::Type::op;
    _cursor = ++marker;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Static
std::string Lexer::typeToString (Lexer::Type type)
{
       if (type == Lexer::Type::string)       return std::string ("\033[38;5;7m\033[48;5;3m")    + "string"       + "\033[0m";
  else if (type == Lexer::Type::uuid)         return std::string ("\033[38;5;7m\033[48;5;10m")   + "uuid"         + "\033[0m";
  else if (type == Lexer::Type::hex)          return std::string ("\033[38;5;7m\033[48;5;14m")   + "hex"          + "\033[0m";
  else if (type == Lexer::Type::number)       return std::string ("\033[38;5;7m\033[48;5;6m")    + "number"       + "\033[0m";
  else if (type == Lexer::Type::url)          return std::string ("\033[38;5;7m\033[48;5;4m")    + "url"          + "\033[0m";
  else if (type == Lexer::Type::path)         return std::string ("\033[37;102m")                + "path"         + "\033[0m";
  else if (type == Lexer::Type::pattern)      return std::string ("\033[37;42m")                 + "pattern"      + "\033[0m";
  else if (type == Lexer::Type::op)           return std::string ("\033[38;5;7m\033[48;5;203m")  + "op"           + "\033[0m";
  else if (type == Lexer::Type::word)         return std::string ("\033[38;5;15m\033[48;5;236m") + "word"         + "\033[0m";
  else if (type == Lexer::Type::date)         return std::string ("\033[38;5;15m\033[48;5;34m")  + "date"         + "\033[0m";
  else if (type == Lexer::Type::duration)     return std::string ("\033[38;5;15m\033[48;5;34m")  + "duration"     + "\033[0m";
  else                                        return std::string ("\033[37;41m")                 + "unknown"      + "\033[0m";
}

////////////////////////////////////////////////////////////////////////////////
// Full implementation of a quoted word.  Includes:
//   '\''
//   '"'
//   "'"
//   "\""
//   'one two'
// Result includes the quotes.
bool Lexer::readWord (
  const std::string& text,
  const std::string& quotes,
  std::string::size_type& cursor,
  std::string& word)
{
  if (quotes.find (text[cursor]) == std::string::npos)
    return false;

  std::string::size_type eos = text.length ();
  int quote = text[cursor++];
  word = quote;

  int c;
  while ((c = text[cursor]))
  {
    // Quoted word ends on a quote.
    if (quote && quote == c)
    {
      word += utf8_character (utf8_next_char (text, cursor));
      break;
    }

    // Unicode U+XXXX or \uXXXX codepoint.
    else if (eos - cursor >= 6 &&
             ((text[cursor + 0] == 'U'  && text[cursor + 1] == '+') ||
              (text[cursor + 0] == '\\' && text[cursor + 1] == 'u')) &&
             unicodeHexDigit (text[cursor + 2]) &&
             unicodeHexDigit (text[cursor + 3]) &&
             unicodeHexDigit (text[cursor + 4]) &&
             unicodeHexDigit (text[cursor + 5]))
    {
      word += utf8_character (
                hexToInt (
                  text[cursor + 2],
                  text[cursor + 3],
                  text[cursor + 4],
                  text[cursor + 5]));
      cursor += 6;
    }

    // An escaped thing.
    else if (c == '\\')
    {
      c = text[++cursor];

      switch (c)
      {
      case '"':  word += (char) 0x22; ++cursor; break;
      case '\'': word += (char) 0x27; ++cursor; break;
      case '\\': word += (char) 0x5C; ++cursor; break;
      case 'b':  word += (char) 0x08; ++cursor; break;
      case 'f':  word += (char) 0x0C; ++cursor; break;
      case 'n':  word += (char) 0x0A; ++cursor; break;
      case 'r':  word += (char) 0x0D; ++cursor; break;
      case 't':  word += (char) 0x09; ++cursor; break;
      case 'v':  word += (char) 0x0B; ++cursor; break;

      // This pass-through default case means that anything can be escaped
      // harmlessly. In particular 'quote' is included, if it not one of the
      // above characters.
      default:   word += (char) c;    ++cursor; break;
      }
    }

    // Ordinary character.
    else
      word += utf8_character (utf8_next_char (text, cursor));
  }

  // Verify termination.
  return word[0]                  == quote &&
         word[word.length () - 1] == quote &&
         word.length () >= 2;
}

////////////////////////////////////////////////////////////////////////////////
// Full implementation of an unquoted word.  Includes:
//   one\ two
//   abcU+0020def
//   abc\u0020def
//   a\tb
//
// Ends at:
//   Lexer::isEOS
//   unicodeWhitespace
//   Lexer::isHardBoundary
bool Lexer::readWord (
  const std::string& text,
  std::string::size_type& cursor,
  std::string& word)
{
  std::string::size_type eos = text.length ();

  word = "";
  int c;
  int prev = 0;
  while ((c = text[cursor]))  // Handles EOS.
  {
    // Unquoted word ends on white space.
    if (unicodeWhitespace (c))
      break;

    // Parentheses mostly.
    if (prev && Lexer::isHardBoundary (prev, c))
      break;

    // Unicode U+XXXX or \uXXXX codepoint.
    else if (eos - cursor >= 6 &&
             ((text[cursor + 0] == 'U'  && text[cursor + 1] == '+') ||
              (text[cursor + 0] == '\\' && text[cursor + 1] == 'u')) &&
             unicodeHexDigit (text[cursor + 2]) &&
             unicodeHexDigit (text[cursor + 3]) &&
             unicodeHexDigit (text[cursor + 4]) &&
             unicodeHexDigit (text[cursor + 5]))
    {
      word += utf8_character (
                hexToInt (
                  text[cursor + 2],
                  text[cursor + 3],
                  text[cursor + 4],
                  text[cursor + 5]));
      cursor += 6;
    }

    // An escaped thing.
    else if (c == '\\')
    {
      c = text[++cursor];

      switch (c)
      {
      case '"':  word += (char) 0x22; ++cursor; break;
      case '\'': word += (char) 0x27; ++cursor; break;
      case '\\': word += (char) 0x5C; ++cursor; break;
      case 'b':  word += (char) 0x08; ++cursor; break;
      case 'f':  word += (char) 0x0C; ++cursor; break;
      case 'n':  word += (char) 0x0A; ++cursor; break;
      case 'r':  word += (char) 0x0D; ++cursor; break;
      case 't':  word += (char) 0x09; ++cursor; break;
      case 'v':  word += (char) 0x0B; ++cursor; break;

      // This pass-through default case means that anything can be escaped
      // harmlessly. In particular 'quote' is included, if it not one of the
      // above characters.
      default:   word += (char) c;    ++cursor; break;
      }
    }

    // Ordinary character.
    else
      word += utf8_character (utf8_next_char (text, cursor));

    prev = c;
  }

  return word.length () > 0 ? true : false;
}

////////////////////////////////////////////////////////////////////////////////
