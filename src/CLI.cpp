////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2006 - 2016, Paul Beckingham, Federico Hernandez.
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
#include <CLI.h>
#include <sstream>
#include <algorithm>
#include <Color.h>
#include <shared.h>

////////////////////////////////////////////////////////////////////////////////
A2::A2 (const std::string& raw, Lexer::Type lextype)
{
  _lextype = lextype;
  attribute ("raw", raw);
}

////////////////////////////////////////////////////////////////////////////////
bool A2::hasTag (const std::string& tag) const
{
  return std::find (_tags.begin (), _tags.end (), tag) != _tags.end ();
}

////////////////////////////////////////////////////////////////////////////////
void A2::tag (const std::string& tag)
{
  if (! hasTag (tag))
    _tags.push_back (tag);
}

////////////////////////////////////////////////////////////////////////////////
void A2::unTag (const std::string& tag)
{
  for (auto i = _tags.begin (); i != _tags.end (); ++i)
    if (*i == tag)
    {
      _tags.erase (i);
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Accessor for attributes.
void A2::attribute (const std::string& name, const std::string& value)
{
  _attributes[name] = value;
}

////////////////////////////////////////////////////////////////////////////////
// Accessor for attributes.
std::string A2::attribute (const std::string& name) const
{
  // Prevent autovivification.
  auto i = _attributes.find (name);
  if (i != _attributes.end ())
    return i->second;

  return "";
}

////////////////////////////////////////////////////////////////////////////////
std::string A2::getToken () const
{
  auto i = _attributes.find ("canonical");
  if (i == _attributes.end ())
    i = _attributes.find ("raw");

  return i->second;
}

////////////////////////////////////////////////////////////////////////////////
std::string A2::dump () const
{
  auto output = Lexer::typeToString (_lextype);

  // Dump attributes.
  std::string atts;
  for (auto& a : _attributes)
    atts += a.first + "='\033[33m" + a.second + "\033[0m' ";

  // Dump tags.
  std::string tags;
  for (auto& tag : _tags)
  {
         if (tag == "BINARY")        tags += "\033[1;37;44m"           + tag + "\033[0m ";
    else if (tag == "CMD")           tags += "\033[1;37;46m"           + tag + "\033[0m ";
    else if (tag == "EXT")           tags += "\033[1;37;42m"           + tag + "\033[0m ";
    else if (tag == "HINT")          tags += "\033[1;37;43m"           + tag + "\033[0m ";
    else if (tag == "FILTER")        tags += "\033[1;37;45m"           + tag + "\033[0m ";
    else                             tags += "\033[32m"                + tag + "\033[0m ";
  }

  return output + " " + atts + tags;
}

////////////////////////////////////////////////////////////////////////////////
void CLI::entity (const std::string& category, const std::string& name)
{
  // Walk the list of entities for category.
  auto c = _entities.equal_range (category);
  for (auto e = c.first; e != c.second; ++e)
    if (e->second == name)
      return;

  // The category/name pair was not found, therefore add it.
  _entities.insert (std::pair <std::string, std::string> (category, name));
}

////////////////////////////////////////////////////////////////////////////////
// Capture a single argument.
void CLI::add (const std::string& argument)
{
  A2 arg (Lexer::trim (argument), Lexer::Type::word);
  arg.tag ("ORIGINAL");
  _original_args.push_back (arg);

  // Adding a new argument invalidates prior analysis.
  _args.clear ();
}

////////////////////////////////////////////////////////////////////////////////
// Arg0 is the first argument, which is the name and potentially a relative or
// absolute path to the invoked binary.
//
// The binary name is 'timew', but if the binary is reported as 'foo' then it
// was invoked via symbolic link, in which case capture the first argument as
// 'foo'. This should allow any command/extension to do this.
//
void CLI::handleArg0 ()
{
  // Capture arg0 separately, because it is the command that was run, and could
  // need special handling.
  auto raw = _original_args[0].attribute ("raw");
  A2 a (raw, Lexer::Type::word);
  a.tag ("BINARY");

  std::string basename = "timew";
  auto slash = raw.rfind ('/');
  if (slash != std::string::npos)
    basename = raw.substr (slash + 1);

  a.attribute ("basename", basename);
  if (basename != "timew")
  {
    A2 cal (basename, Lexer::Type::word);
    _args.push_back (cal);
  }

  _args.push_back (a);
}

////////////////////////////////////////////////////////////////////////////////
// All arguments must be individually and wholly recognized by the Lexer. Any
// argument not recognized is considered a Lexer::Type::word.
void CLI::lexArguments ()
{
  // Note: Starts iterating at index 1, because ::handleArg0 has already
  //       processed 0.
  for (unsigned int i = 1; i < _original_args.size (); ++i)
  {
    bool quoted = Lexer::wasQuoted (_original_args[i].attribute ("raw"));

    std::string lexeme;
    Lexer::Type type;
    Lexer lex (_original_args[i].attribute ("raw"));
    if (lex.token (lexeme, type) &&
        lex.isEOS ())
    {
      A2 a (Lexer::dequote (_original_args[i].attribute ("raw")), type);
      if (quoted)
        a.tag ("QUOTED");

      if (_original_args[i].hasTag ("ORIGINAL"))
        a.tag ("ORIGINAL");

      _args.push_back (a);
    }
    else
    {
      std::string quote = "'";
      auto escaped = str_replace (_original_args[i].attribute ("raw"), quote, "\\'");

      std::string::size_type cursor = 0;
      std::string word;
      if (Lexer::readWord (quote + escaped + quote, quote, cursor, word))
      {
        A2 unknown (Lexer::dequote (word), Lexer::Type::word);
        if (Lexer::wasQuoted (_original_args[i].attribute ("raw")))
          unknown.tag ("QUOTED");

        if (_original_args[i].hasTag ("ORIGINAL"))
          unknown.tag ("ORIGINAL");

        _args.push_back (unknown);
      }

      // This branch may have no use-case.
      else
      {
        A2 unknown (Lexer::dequote (_original_args[i].attribute ("raw")), Lexer::Type::word);
        unknown.tag ("UNKNOWN");

        if (Lexer::wasQuoted (_original_args[i].attribute ("raw")))
          unknown.tag ("QUOTED");

        if (_original_args[i].hasTag ("ORIGINAL"))
          unknown.tag ("ORIGINAL");

        _args.push_back (unknown);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Intended to be called after ::add() to perform the final analysis.
void CLI::analyze ()
{
  // Process _original_args.
  _args.clear ();
  handleArg0 ();
  lexArguments ();
  canonicalizeNames ();
  identifyFilter ();
}

////////////////////////////////////////////////////////////////////////////////
// Return all the unknown args.
std::vector <std::string> CLI::getWords () const
{
  std::vector <std::string> words;
  for (auto& a : _args)
    if (! a.hasTag ("BINARY") &&
        ! a.hasTag ("CMD")    &&
        ! a.hasTag ("HINT"))
      words.push_back (a.attribute ("raw"));

  return words;
}

////////////////////////////////////////////////////////////////////////////////
// Search for 'value' in _entities category, return canonicalized value.
bool CLI::canonicalize (
  std::string& canonicalized,
  const std::string& category,
  const std::string& value) const
{
  // Extract a list of entities for category.
  std::vector <std::string> options;
  auto c = _entities.equal_range (category);
  for (auto e = c.first; e != c.second; ++e)
  {
    // Shortcut: if an exact match is found, success.
    if (value == e->second)
    {
      canonicalized = value;
      return true;
    }

    options.push_back (e->second);
  }

  // Match against the options, throw away results.
  std::vector <std::string> matches;
  if (autoComplete (value, options, matches) == 1)
  {
    canonicalized = matches[0];
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
std::string CLI::getBinary () const
{
  if (_args.size ())
    return _args[0].attribute ("raw");

  return "";
}

////////////////////////////////////////////////////////////////////////////////
std::string CLI::getCommand () const
{
  for (auto& a : _args)
    if (a.hasTag ("CMD"))
      return a.attribute ("canonical");

  for (auto& a : _args)
    if (a.hasTag ("EXT"))
      return a.attribute ("canonical");

  return "";
}

////////////////////////////////////////////////////////////////////////////////
std::string CLI::dump (const std::string& title) const
{
  std::stringstream out;

  out << "\033[1m" << title << "\033[0m\n"
      << "  _original_args\n    ";

  Color colorArgs ("gray10 on gray4");
  for (auto i = _original_args.begin (); i != _original_args.end (); ++i)
  {
    if (i != _original_args.begin ())
      out << ' ';

    out << colorArgs.colorize (i->attribute ("raw"));
  }

  out << '\n';

  if (_args.size ())
  {
    out << "  _args\n";
    for (auto& a : _args)
      out << "    " << a.dump () << '\n';
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Scan all arguments and canonicalize names that need it.
void CLI::canonicalizeNames ()
{
  bool alreadyFoundCmd = false;

  for (auto& a : _args)
  {
    auto raw = a.attribute ("raw");
    std::string canonical = raw;

    // Commands.
    if (! alreadyFoundCmd &&
        (exactMatch ("command", raw) ||
         canonicalize (canonical, "command", raw)))
    {
      a.attribute ("canonical", canonical);
      a.tag ("CMD");
      alreadyFoundCmd = true;
    }

    // Hints.
    else if (exactMatch ("hint", raw) ||
             canonicalize (canonical, "hint", raw))
    {
      a.attribute ("canonical", canonical);
      a.tag ("HINT");
    }

    // Extensions.
    else if (! alreadyFoundCmd &&
             (exactMatch ("extension", raw) ||
              canonicalize (canonical, "extension", raw)))
    {
      a.attribute ("canonical", canonical);
      a.tag ("EXT");
      alreadyFoundCmd = true;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Locate arguments that are part of a filter.
void CLI::identifyFilter ()
{
  for (auto& a : _args)
  {
    auto raw = a.attribute ("raw");

    if (a.hasTag ("HINT"))
      a.tag ("FILTER");

    else if (a._lextype == Lexer::Type::date ||
             a._lextype == Lexer::Type::duration)
    {
      a.tag ("FILTER");
    }

    else if (raw == "from"  ||
             raw == "since" ||
             raw == "to"    ||
             raw == "until" ||
             raw == "-"     ||
             raw == "for")
    {
      a.tag ("FILTER");
      a.tag ("KEYWORD");
    }

    else if (! a.hasTag ("CMD") &&
             ! a.hasTag ("EXT") &&
             ! a.hasTag ("BINARY"))
    {
      a.tag ("FILTER");
      a.tag ("TAG");
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Search for exact 'value' in _entities category.
bool CLI::exactMatch (
  const std::string& category,
  const std::string& value) const
{
  // Extract a list of entities for category.
  auto c = _entities.equal_range (category);
  for (auto e = c.first; e != c.second; ++e)
    if (value == e->second)
      return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
