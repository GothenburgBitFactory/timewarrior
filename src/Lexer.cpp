////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013 - 2015, Paul Beckingham, Federico Hernandez.
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
#include <algorithm>
#include <ctype.h>
#include <utf8.h>

////////////////////////////////////////////////////////////////////////////////
Lexer::Lexer (const std::string& text)
: _text (text)
, _cursor (0)
, _eos (text.size ())
{
}

////////////////////////////////////////////////////////////////////////////////
// When a Lexer object is constructed with a string, this method walks through
// the stream of low-level tokens.
bool Lexer::token (std::string& token, Lexer::Type& type)
{
  // Eat white space.
  while (isWhitespace (_text[_cursor]))
    utf8_next_char (_text, _cursor);

  // Terminate at EOS.
  if (isEOS ())
    return false;

  if (isString    (token, type, "'\"") ||
      isURL       (token, type)        ||
      isHexNumber (token, type)        ||
      isNumber    (token, type)        ||
      isPath      (token, type)        ||
      isPattern   (token, type)        ||
      isWord      (token, type))
    return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
// No L10N - these are for internal purposes.
const std::string Lexer::typeName (const Lexer::Type& type)
{
  switch (type)
  {
  case Lexer::Type::number:       return "number";
  case Lexer::Type::hex:          return "hex";
  case Lexer::Type::string:       return "string";
  case Lexer::Type::url:          return "url";
  case Lexer::Type::path:         return "path";
  case Lexer::Type::pattern:      return "pattern";
  case Lexer::Type::word:         return "word";
  }

  return "unknown";
}

////////////////////////////////////////////////////////////////////////////////
// Complete Unicode whitespace list.
//
// http://en.wikipedia.org/wiki/Whitespace_character
// Updated 2015-09-13
// Static
//
// TODO This list should be derived from the Unicode database.
bool Lexer::isWhitespace (int c)
{
  return (c == 0x0020 ||   // space Common  Separator, space
          c == 0x0009 ||   // Common  Other, control  HT, Horizontal Tab
          c == 0x000A ||   // Common  Other, control  LF, Line feed
          c == 0x000B ||   // Common  Other, control  VT, Vertical Tab
          c == 0x000C ||   // Common  Other, control  FF, Form feed
          c == 0x000D ||   // Common  Other, control  CR, Carriage return
          c == 0x0085 ||   // Common  Other, control  NEL, Next line
          c == 0x00A0 ||   // no-break space  Common  Separator, space
          c == 0x1680 ||   // ogham space mark  Ogham Separator, space
          c == 0x180E ||   // mongolian vowel separator Mongolian Separator, space
          c == 0x2000 ||   // en quad Common  Separator, space
          c == 0x2001 ||   // em quad Common  Separator, space
          c == 0x2002 ||   // en space  Common  Separator, space
          c == 0x2003 ||   // em space  Common  Separator, space
          c == 0x2004 ||   // three-per-em space  Common  Separator, space
          c == 0x2005 ||   // four-per-em space Common  Separator, space
          c == 0x2006 ||   // six-per-em space  Common  Separator, space
          c == 0x2007 ||   // figure space  Common  Separator, space
          c == 0x2008 ||   // punctuation space Common  Separator, space
          c == 0x2009 ||   // thin space  Common  Separator, space
          c == 0x200A ||   // hair space  Common  Separator, space
          c == 0x200B ||   // zero width space
          c == 0x200C ||   // zero width non-joiner
          c == 0x200D ||   // zero width joiner
          c == 0x2028 ||   // line separator  Common  Separator, line
          c == 0x2029 ||   // paragraph separator Common  Separator, paragraph
          c == 0x202F ||   // narrow no-break space Common  Separator, space
          c == 0x205F ||   // medium mathematical space Common  Separator, space
          c == 0x2060 ||   // word joiner
          c == 0x3000);    // ideographic space Common  Separator, space
}

////////////////////////////////////////////////////////////////////////////////
// Digits 0-9.
//
// TODO This list should be derived from the Unicode database.
bool Lexer::isDigit (int c)
{
  return c >= 0x30 && c <= 0x39;
}

////////////////////////////////////////////////////////////////////////////////
// Digits 0-9 a-f A-F.
bool Lexer::isHexDigit (int c)
{
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
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

  if (isDigit (_text[marker]))
  {
    ++marker;
    while (isDigit (_text[marker]))
      utf8_next_char (_text, marker);

    if (_text[marker] == '.')
    {
      ++marker;
      if (isDigit (_text[marker]))
      {
        ++marker;
        while (isDigit (_text[marker]))
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

      if (isDigit (_text[marker]))
      {
        ++marker;
        while (isDigit (_text[marker]))
          utf8_next_char (_text, marker);

        if (_text[marker] == '.')
        {
          ++marker;
          if (isDigit (_text[marker]))
          {
            ++marker;
            while (isDigit (_text[marker]))
              utf8_next_char (_text, marker);
          }
        }
      }
    }

    // Lookahread: !<isWhitespace> | !<isSingleCharOperator>
    // If there is an immediately consecutive character, that is not an operator, fail.
    if (_eos > marker &&
        ! isWhitespace (_text[marker]) &&
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

  if (isDigit (_text[marker]))
  {
    ++marker;
    while (isDigit (_text[marker]))
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

    while (isHexDigit (_text[marker]))
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
         ! isWhitespace (_text[marker]) &&
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
             ! isWhitespace (_text[marker]))
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
        ! isWhitespace (_text[marker]) &&
        _text[marker] != '/')
    {
      utf8_next_char (_text, marker);
      while (_text[marker] &&
             ! isWhitespace (_text[marker]) &&
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
//   / <unquoted-string> /  <EOS> | <isWhitespace>
bool Lexer::isPattern (std::string& token, Lexer::Type& type)
{
  std::size_t marker = _cursor;

  std::string word;
  if (readWord (_text, "/", _cursor, word) &&
      (isEOS () ||
       isWhitespace (_text[_cursor])))
  {
    token = _text.substr (marker, _cursor - marker);
    type = Lexer::Type::pattern;
    return true;
  }

  _cursor = marker;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Static
std::string Lexer::typeToString (Lexer::Type type)
{
       if (type == Lexer::Type::string)       return std::string ("\033[38;5;7m\033[48;5;3m")    + "string"       + "\033[0m";
  else if (type == Lexer::Type::hex)          return std::string ("\033[38;5;7m\033[48;5;14m")   + "hex"          + "\033[0m";
  else if (type == Lexer::Type::number)       return std::string ("\033[38;5;7m\033[48;5;6m")    + "number"       + "\033[0m";
  else if (type == Lexer::Type::url)          return std::string ("\033[38;5;7m\033[48;5;4m")    + "url"          + "\033[0m";
  else if (type == Lexer::Type::path)         return std::string ("\033[37;102m")                + "path"         + "\033[0m";
  else if (type == Lexer::Type::pattern)      return std::string ("\033[37;42m")                 + "pattern"      + "\033[0m";
  else if (type == Lexer::Type::word)         return std::string ("\033[38;5;15m\033[48;5;236m") + "word"         + "\033[0m";
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
             isHexDigit (text[cursor + 2]) &&
             isHexDigit (text[cursor + 3]) &&
             isHexDigit (text[cursor + 4]) &&
             isHexDigit (text[cursor + 5]))
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
//   Lexer::isWhitespace
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
    if (Lexer::isWhitespace (c))
      break;

    // Parentheses mostly.
    if (prev && Lexer::isHardBoundary (prev, c))
      break;

    // Unicode U+XXXX or \uXXXX codepoint.
    else if (eos - cursor >= 6 &&
             ((text[cursor + 0] == 'U'  && text[cursor + 1] == '+') ||
              (text[cursor + 0] == '\\' && text[cursor + 1] == 'u')) &&
             isHexDigit (text[cursor + 2]) &&
             isHexDigit (text[cursor + 3]) &&
             isHexDigit (text[cursor + 4]) &&
             isHexDigit (text[cursor + 5]))
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
