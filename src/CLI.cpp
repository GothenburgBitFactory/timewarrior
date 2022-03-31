////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 - 2022, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <CLI.h>
#include <Color.h>
#include <DatetimeParser.h>
#include <Duration.h>
#include <Pig.h>
#include <algorithm>
#include <format.h>
#include <set>
#include <shared.h>
#include <sstream>
#include <timew.h>
#include <utf8.h>

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
  {
    if (*i == tag)
    {
      _tags.erase (i);
      break;
    }
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
void A2::attribute (const std::string& name, int value)
{
  _attributes[name] = format ("{1}", value);
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
  for (const auto& tag : _tags)
  {
         if (tag == "BINARY")        tags += "\033[1;37;44m"             + tag + "\033[0m ";
    else if (tag == "CMD")           tags += "\033[1;37;46m"             + tag + "\033[0m ";
    else if (tag == "EXT")           tags += "\033[1;37;42m"             + tag + "\033[0m ";
    else if (tag == "HINT")          tags += "\033[1;37;43m"             + tag + "\033[0m ";
    else if (tag == "FILTER")        tags += "\033[1;37;45m"             + tag + "\033[0m ";
    else if (tag == "CONFIG")        tags += "\033[1;37;101m"            + tag + "\033[0m ";
    else if (tag == "ID")            tags += "\033[38;5;7m\033[48;5;34m" + tag + "\033[0m ";
    else                             tags += "\033[32m"                  + tag + "\033[0m ";
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
  // Sanitize the input: Convert control charts to spaces. Then trim.
  std::string clean;
  std::string::size_type i = 0;
  int character;
  while ((character = utf8_next_char (argument.c_str (), i)))
  {
    if (character <= 32)
       clean += ' ';
    else
       clean += utf8_character (character);
  }

  A2 arg (Lexer::trim (clean), Lexer::Type::word);
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
      A2 a (_original_args[i].attribute ("raw"), type);
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
  identifyOverrides ();
  identifyIds ();
  canonicalizeNames ();
  identifyFilter ();
}

////////////////////////////////////////////////////////////////////////////////
// Return all the unknown args.
std::vector <std::string> CLI::getWords () const
{
  std::vector <std::string> words;
  for (const auto& a : _args)
    if (! a.hasTag ("BINARY") &&
        ! a.hasTag ("CMD")    &&
        ! a.hasTag ("CONFIG") &&
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
  if (! _args.empty ())
    return _args[0].attribute ("raw");

  return "";
}

////////////////////////////////////////////////////////////////////////////////
std::string CLI::getCommand () const
{
  for (const auto& a : _args)
    if (a.hasTag ("CMD"))
      return a.attribute ("canonical");

  for (const auto& a : _args)
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

  if (! _args.empty ())
  {
    out << "  _args\n";
    for (auto& a : _args)
      out << "    " << a.dump () << '\n';
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
// Scan all arguments and identify instances of 'rc.<name>[:=]<value>'.
void CLI::identifyOverrides ()
{
  for (auto& a : _args)
  {
    auto raw = a.attribute ("raw");

    if (raw.length () > 3 &&
        raw.substr (0, 3) == "rc.")
    {
      auto sep = raw.find ('=', 3);
      if (sep == std::string::npos)
        sep = raw.find (':', 3);
      if (sep != std::string::npos)
      {
        a.tag ("CONFIG");
        a.attribute ("name",  raw.substr (3, sep - 3));
        a.attribute ("value", raw.substr (sep + 1));
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Scan all arguments and identify instances of '@<integer>'.
void CLI::identifyIds ()
{
  for (auto& a : _args)
  {
    if (a._lextype == Lexer::Type::word)
    {
      Pig pig (a.attribute ("raw"));
      int digits;
      if (pig.skipLiteral ("@")  &&
          pig.getDigits (digits) &&
          pig.eos ())
      {
        if (digits <= 0)
          throw format ("'@{1}' is not a valid ID.", digits);

        a.tag ("ID");
        a.attribute ("value", digits);
      }
    }
  }
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

    // 'timew <command> --help|-h' should be treated the same as 'timew help <command>'.
    // Therefore, '--help|-h' on the command line should always become the command.
    else if (alreadyFoundCmd && (raw == "--help" || raw == "-h"))
    {
      for (auto& b : _args) {
        if (b.hasTag("CMD"))
        {
          b.unTag("CMD");
          break;
        }
      }

      a.tag ("CMD");
      a.attribute("canonical", canonical);
    }

    // Hints.
    else if (exactMatch ("hint", raw) ||
             canonicalize (canonical, "hint", raw))
    {
      a.attribute ("canonical", canonical);
      a.tag ("HINT");
    }

    // Extensions.
    else if (exactMatch ("extension", raw) ||
             canonicalize (canonical, "extension", raw))
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
    if (a.hasTag ("CMD")    ||
        a.hasTag ("EXT")    ||
        a.hasTag ("CONFIG") ||
        a.hasTag ("BINARY"))
      continue;

    auto raw = a.attribute ("raw");

    if (a.hasTag ("HINT"))
      a.tag ("FILTER");

    else if (a.hasTag ("ID"))
      a.tag ("FILTER");

    else if (a._lextype == Lexer::Type::date ||
             a._lextype == Lexer::Type::duration)
    {
      a.tag ("FILTER");
    }

    else if (raw == "from"   ||
             raw == "since"  ||
             raw == "to"     ||
             raw == "for"    ||
             raw == "until"  ||
             raw == "-"      ||
             raw == "before" ||
             raw == "after"  ||
             raw == "ago")
    {
      a.tag ("FILTER");
      a.tag ("KEYWORD");
    }

    else if (raw.rfind("dom.",0) == 0)
    {
      a.tag ("DOM");
    }
    else
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
std::set<int> CLI::getIds() const
{
  std::set <int> ids;

  for (auto& arg : _args)
  {
    if (arg.hasTag ("ID"))
      ids.insert (strtol (arg.attribute ("value").c_str (), NULL, 10));
  }

  return ids;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> CLI::getTags () const
{
  std::vector <std::string> tags;

  for (auto& arg : _args)
  {
    if (arg.hasTag ("TAG"))
      tags.push_back (arg.attribute ("raw"));
  }

  return tags;
}

////////////////////////////////////////////////////////////////////////////////
std::string CLI::getAnnotation () const
{
  std::string annotation;

  for (auto& arg : _args)
  {
    if (arg.hasTag ("TAG"))
    {
      annotation = (arg.attribute ("raw"));
    }

  }

  return annotation;
}

////////////////////////////////////////////////////////////////////////////////
Duration CLI::getDuration () const
{
  std::string delta;
  for (auto& arg : _args)
  {
    if (arg.hasTag ("FILTER") &&
        arg._lextype == Lexer::Type::duration)
    {
      delta = arg.attribute ("raw");
    }
  }
  Duration dur (delta);
  return dur;
}

////////////////////////////////////////////////////////////////////////////////

std::vector <std::string> CLI::getDomReferences () const
{
  std::vector<std::string> references;

  for (auto &arg : _args)
  {
    if (arg.hasTag ("DOM"))
    {
      references.emplace_back (arg.attribute ("raw"));
    }
  }

  return references;
}

////////////////////////////////////////////////////////////////////////////////
// A filter is just another interval, containing start, end and tags.
//
// Supported interval forms:
//   ["from"] <date> ["to"|"-" <date>]
//   ["from"] <date> "for" <duration>
//   <duration> ["before"|"after" <date>]
//   <duration> "ago"
//
Interval CLI::getFilter (const Range& default_range) const
{
  // One instance, so we can directly compare.
  Datetime now;

  Interval filter;
  std::string start;
  std::string end;
  std::string duration;
  std::vector <std::string> args;

  for (auto& arg : _args)
  {
    if (arg.hasTag ("BINARY") ||
        arg.hasTag ("CMD")    ||
        arg.hasTag ("EXT"))
      continue;

    if (arg.hasTag ("FILTER"))
    {
      auto canonical = arg.attribute ("canonical");
      auto raw       = arg.attribute ("raw");

      if (arg.hasTag ("HINT"))
      {
        Range range;
        if (expandIntervalHint (canonical, range))
        {
          if (range.is_empty ())
          {
            args.emplace_back("<all>");
          }
          else
          {
            start = range.start.toISO ();
            end   = range.end.toISO ();

            args.emplace_back("<date>");
            args.emplace_back("-");
            args.emplace_back("<date>");
          }
        }

        // Hints that are not expandable to a date range are ignored.
      }
      else if (arg._lextype == Lexer::Type::date)
      {
        if (start.empty ())
          start = raw;
        else if (end.empty ())
          end = raw;

        args.emplace_back("<date>");
      }
      else if (arg._lextype == Lexer::Type::duration)
      {
        if (duration.empty ())
          duration = raw;

        args.emplace_back("<duration>");
      }
      else if (arg.hasTag ("KEYWORD"))
      {
        // Note: that KEYWORDS are not entities (why not?) and there is a list
        //       in CLI.cpp of them that must be maintained and synced with this
        //       function.
        args.push_back (raw);
      }
      else if (arg.hasTag ("ID"))
      {
        // Not part of a filter.
      }
      else
      {
        filter.tag (raw);
      }
    }
  }

  if (args.empty ())
  {
    filter.setRange(default_range);
  }

  // <date>
  else if (args.size () == 1 &&
      args[0] == "<date>")
  {
    DatetimeParser dtp;
    Range range = dtp.parse_range(start);
    filter.setRange (range);
  }

  // from <date>
  else if (args.size () == 2 &&
           args[0] == "from" &&
           args[1] == "<date>")
  {
    filter.setRange ({Datetime (start), 0});
  }
  // <date> to/- <date>
  else if (args.size () == 3                   &&
           args[0] == "<date>"                 &&
           (args[1] == "to" || args[1] == "-") &&
           args[2] == "<date>")
  {
    filter.setRange ({Datetime (start), Datetime (end)});
  }

  // from <date> to/- <date>
  else if (args.size () == 4                   &&
           args[0] == "from"                   &&
           args[1] == "<date>"                 &&
           (args[2] == "to" || args[2] == "-") &&
           args[3] == "<date>")
  {
    filter.setRange ({Datetime (start), Datetime (end)});
  }

  // <date> for <duration>
  else if (args.size () == 3   &&
           args[0] == "<date>" &&
           args[1] == "for"    &&
           args[2] == "<duration>")
  {
    filter.setRange ({Datetime (start), Datetime (start) + Duration (duration).toTime_t ()});
  }

  // from <date> for <duration>
  else if (args.size () == 4       &&
           args[0] == "from"       &&
           args[1] == "<date>"     &&
           args[2] == "for"        &&
           args[3] == "<duration>")
  {
    filter.setRange ({Datetime (start), Datetime (start) + Duration (duration).toTime_t ()});
  }

  // <duration> before <date>
  else if (args.size () == 3       &&
           args[0] == "<duration>" &&
           args[1] == "before"     &&
           args[2] == "<date>")
  {
    filter.setRange ({Datetime (start) - Duration (duration).toTime_t (), Datetime (start)});
  }

  // <duration> after <date>
  else if (args.size () == 3       &&
           args[0] == "<duration>" &&
           args[1] == "after"      &&
           args[2] == "<date>")
  {
    filter.setRange ({Datetime (start), Datetime (start) + Duration (duration).toTime_t ()});
  }

  // <duration> ago
  else if (args.size () == 2       &&
           args[0] == "<duration>" &&
           args[1] == "ago")
  {
    filter.setRange ({now - Duration (duration).toTime_t (), 0});
  }

  // for <duration>
  else if (args.size () == 2       &&
           args[0] == "for"        &&
           args[1] == "<duration>")
  {
    filter.setRange ({now - Duration (duration).toTime_t (), now});
  }

  // <duration>
  else if (args.size () == 1 &&
           args[0] == "<duration>")
  {
    filter.setRange ({now - Duration (duration).toTime_t (), now});
  }

  // :all
  else if (args.size () == 1 && args[0] == "<all>")
  {
    filter.setRange (0, 0);
  }

    // Unrecognized date range construct.
  else if (! args.empty ())
  {
    throw std::string ("Unrecognized date range: '") + join (" ", args) + "'.";
  }

  if (filter.end != 0 && filter.start > filter.end)
    throw std::string ("The end of a date range must be after the start.");

  return filter;
}

////////////////////////////////////////////////////////////////////////////////
bool CLI::findHint (const std::string& hint) const
{
  for (auto& arg : _args)
  {
    if (arg.hasTag ("HINT") &&
        arg.getToken () == ":" + hint)
    {
      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
bool CLI::getComplementaryHint (const std::string& base, const bool default_value) const
{
  if (findHint (base))
  {
    return true;
  }
  else if (findHint ("no-" + base))
  {
    return false;
  }

  return default_value;
}

////////////////////////////////////////////////////////////////////////////////
bool CLI::getHint (const std::string &base, const bool default_value) const
{
  if (findHint (base))
  {
    return true;
  }

  return default_value;
}
