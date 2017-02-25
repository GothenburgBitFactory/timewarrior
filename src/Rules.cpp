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
#include <Rules.h>
#include <FS.h>
#include <shared.h>
#include <format.h>
#include <sstream>
#include <tuple>
#include <cassert>
#include <cerrno>
#include <inttypes.h>

////////////////////////////////////////////////////////////////////////////////
Rules::Rules ()
{
  // Load the default values.
  _settings =
  {
    {"confirmation",             "on"},
    {"debug",                    "off"},
    {"verbose",                  "on"},

    // 'day' report.
    {"reports.day.hours",        "all"},
    {"reports.day.axis",         "internal"},
    {"reports.day.lines",        "2"},
    {"reports.day.spacing",      "1"},
    {"reports.day.month",        "no"},
    {"reports.day.week",         "no"},
    {"reports.day.day",          "yes"},
    {"reports.day.weekday",      "yes"},
    {"reports.day.totals",       "no"},
    {"reports.day.summary",      "yes"},
    {"reports.day.holidays",     "no"},
    {"reports.day.cell",         "15"},

    // 'week' report.
    {"reports.week.hours",       "all"},
    {"reports.week.lines",       "1"},
    {"reports.week.spacing",     "1"},
    {"reports.week.month",       "no"},
    {"reports.week.week",        "yes"},
    {"reports.week.day",         "yes"},
    {"reports.week.weekday",     "yes"},
    {"reports.week.totals",      "yes"},
    {"reports.week.summary",     "yes"},
    {"reports.week.holidays",    "yes"},
    {"reports.week.cell",        "15"},

    // 'month' report.
    {"reports.month.hours",      "all"},
    {"reports.month.lines",      "1"},
    {"reports.month.spacing",    "1"},
    {"reports.month.month",      "yes"},
    {"reports.month.week",       "yes"},
    {"reports.month.day",        "yes"},
    {"reports.month.weekday",    "yes"},
    {"reports.month.totals",     "yes"},
    {"reports.month.summary",    "yes"},
    {"reports.month.holidays",   "yes"},
    {"reports.month.cell",       "15"},

    // 'summary' report.
    {"reports.summary.holidays", "yes"},

    // 'gaps' report.
    {"reports.gaps.range",       ":day"},

    // Enough of a theme to make the charts work.
    {"theme.description",        "Built-in default"},
    {"theme.colors.exclusion",   "gray8 on gray4"},
    {"theme.colors.today",       "white"},
    {"theme.colors.holiday",     "gray4"},
    {"theme.colors.label",       "gray4"},
  };
}

////////////////////////////////////////////////////////////////////////////////
// Nested files are supported, with the following construct:
//   import /absolute/path/to/file
void Rules::load (const std::string& file, int nest /* = 1 */)
{
  if (nest > 10)
    throw std::string ("Rules files may only be nested to 10 levels.");

  if (nest == 1)
  {
    File originalFile (file);
    _original_file = originalFile._data;

    if (! originalFile.exists ())
      throw std::string ("ERROR: Configuration file not found.");

    if (! originalFile.readable ())
      throw std::string ("ERROR: Configuration file cannot be read (insufficient privileges).");
  }

  // Read the file, then parse the contents.
  std::string contents;
  if (File::read (file, contents) && contents.length ())
    parse (contents, nest);
}

////////////////////////////////////////////////////////////////////////////////
std::string Rules::file () const
{
  return _original_file;
}

////////////////////////////////////////////////////////////////////////////////
bool Rules::has (const std::string& key) const
{
  return _settings.find (key) != _settings.end ();
}

////////////////////////////////////////////////////////////////////////////////
// Return the configuration value given the specified key.
std::string Rules::get (const std::string& key) const
{
  auto found = _settings.find (key);
  if (found != _settings.end ())
    return found->second;

  return "";
}

////////////////////////////////////////////////////////////////////////////////
int Rules::getInteger (const std::string& key, int defaultValue) const
{
  auto found = _settings.find (key);
  if (found != _settings.end ())
  {
    int value = strtoimax (found->second.c_str (), nullptr, 10);

    // Invalid values are handled.  ERANGE errors are simply capped by
    // strtoimax, which is desired behavior.
    if (value == 0 && errno == EINVAL)
      throw format ("Invalid integer value for '{1}': '{2}'", key, found->second);

    return value;
  }

  return defaultValue;
}

////////////////////////////////////////////////////////////////////////////////
double Rules::getReal (const std::string& key) const
{
  auto found = _settings.find (key);
  if (found != _settings.end ())
    return strtod (found->second.c_str (), nullptr);

  return 0.0;
}

////////////////////////////////////////////////////////////////////////////////
bool Rules::getBoolean (const std::string& key) const
{
  auto found = _settings.find (key);
  if (found != _settings.end ())
  {
    auto value = lowerCase (found->second);
    if (value == "true"   ||
        value == "1"      ||
        value == "y"      ||
        value == "yes"    ||
        value == "on")
      return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
void Rules::set (const std::string& key, const int value)
{
  _settings[key] = format (value);
}

////////////////////////////////////////////////////////////////////////////////
void Rules::set (const std::string& key, const double value)
{
  _settings[key] = format (value, 1, 8);
}

////////////////////////////////////////////////////////////////////////////////
void Rules::set (const std::string& key, const std::string& value)
{
  _settings[key] = value;
}

////////////////////////////////////////////////////////////////////////////////
// Provide a vector of all configuration keys. If a stem is provided, only
// return matching keys.
std::vector <std::string> Rules::all (const std::string& stem) const
{
  std::vector <std::string> items;
  for (auto& it : _settings)
    if (stem == "" || it.first.find (stem) == 0)
      items.push_back (it.first);

  return items;
}

////////////////////////////////////////////////////////////////////////////////
bool Rules::isRuleType (const std::string& type) const
{
  if (std::find (_rule_types.begin (), _rule_types.end (), type) != _rule_types.end ())
    return true;

  return false;
}

////////////////////////////////////////////////////////////////////////////////
std::string Rules::dump () const
{
  std::stringstream out;
  out << "Rules\n"
      << "  _original_file " << _original_file
      << '\n';

  out << "  Settings\n";
  for (auto& item : _settings)
    out << "    " << item.first << "=" << item.second << '\n';

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
void Rules::parse (const std::string& input, int nest /* = 1 */)
{
  bool inRule = false;
  std::string ruleDef;

  // Remove comments from input.
  for (auto& line : split (input, '\n'))
  {
    auto comment = line.find ("#");
    if (comment != std::string::npos)
    {
      line.resize (comment);
      line = rtrim (line);
    }

    // Tokenize the line for convenience, but capture the indentation level.
    // tokens.
    auto indent = line.find_first_not_of (' ');

    auto tokens = Lexer::tokenize (line);
    if (tokens.size ())
    {
      if (inRule)
      {
        if (indent == 0)
        {
          inRule = false;
          parseRule (ruleDef);
          ruleDef = "";
        }
        else
        {
          ruleDef += line + '\n';
        }
      }

      // Note: this should NOT be an 'else' to the above 'if (inRule)', because
      //       there are lines where both blocks need to run.
      if (! inRule)
      {
        auto firstWord = std::get <0> (tokens[0]);

        // Top-level rule definition:
        //   'define'
        //      ...
        //
        if (firstWord == "define")
        {
          if (ruleDef != "")
          {
            parseRule (ruleDef);
            ruleDef = "";
          }

          inRule = true;
          ruleDef = line + '\n';
        }

        // Top-level import:
        //   'import' <file>
        else if (firstWord == "import" &&
                 tokens.size () == 2   &&
                 std::get <1> (tokens[1]) == Lexer::Type::path)
        {
          File imported (std::get <0> (tokens[1]));
          if (! imported.is_absolute ())
            throw format ("Can only import files with absolute paths, not '{1}'.", imported._data);

          if (! imported.readable ())
            throw format ("Could not read imported file '{1}'.", imported._data);

          load (imported._data, nest + 1);
        }

        // Top-level settings:
        //   <name> '=' <value>
        else if (tokens.size () >= 3 &&
                 std::get <0> (tokens[1]) == "=")
        {
          // If this line is an assignment, then tokenizing it is a mistake, so
          // use the raw data from 'line'.
          auto equals = line.find ('=');
          assert (equals != std::string::npos);

          set (trim (line.substr (indent, equals - indent)),
               trim (line.substr (equals + 1)));
        }

        // Top-level settings, with no value:
        //   <name> '='
        else if (tokens.size () == 2 &&
                 std::get <0> (tokens[1]) == "=")
        {
          set (firstWord, "");
        }

        // Admit defeat.
        else
          throw format ("Unrecognized construct: {1}", line);
      }
    }
  }

  if (ruleDef != "")
    parseRule (ruleDef);
}

////////////////////////////////////////////////////////////////////////////////
void Rules::parseRule (const std::string& input)
{
  // Break the rule def into lines.
  auto lines = split (input, '\n');

  // Tokenize the first line.
  std::vector <std::string> tokens;
  std::string token;
  Lexer::Type type;
  Lexer lexer (lines[0]);
  while (lexer.token (token, type))
    tokens.push_back (token);

  // Based on the tokens of the first line, determine which rule type needs to
  // be parsed.
  if (tokens.size () >= 2 &&
      tokens[0] == "define")
  {
    if (tokens.size () >= 2 &&
        isRuleType (tokens[1].substr (0, tokens[1].length () - 1)))
      parseRuleSettings (lines);

    // Error.
    else
      throw format ("Unrecognized rule type '{1}'", join (" ", tokens));
  }
}

////////////////////////////////////////////////////////////////////////////////
void Rules::parseRuleSettings (
  const std::vector <std::string>& lines)
{
  std::vector <std::string> hierarchy;
  std::vector <unsigned int> indents;
  indents.push_back (0);

  for (auto& line : lines)
  {
    auto indent = getIndentation (line);
    auto tokens = tokenizeLine (line);
    auto group = parseGroup (tokens);

    // Capture increased indentation.
    if (indent > indents.back ())
      indents.push_back (indent);

    // If indent decreased.
    else if (indent < indents.back ())
    {
      while (indents.size () && indent != indents.back ())
      {
        indents.pop_back ();
        hierarchy.pop_back ();
      }

      // Spot raggedy-ass indentation.
      if (indent != indents.back ())
        throw std::string ("Syntax error in rule: mismatched indent.");
    }

    // Descend.
    if (group != "")
      hierarchy.push_back (group);

    // Settings.
    if (tokens.size () >= 3 && tokens[1] == "=")
    {
      auto name  = join (".", hierarchy) + "." + tokens[0];

      auto equals = line.find ('=');
      if (equals == std::string::npos)
        throw format ("Syntax error in rule: missing '=' in line '{1}'.", line);

      auto value = Lexer::dequote (trim (line.substr (equals + 1)));
      set (name, value);
    }
  }

  // Should arrive here with indents and hierarchy in their original state.
  if (indents.size () != 1 ||
      indents[0] != 0)
    throw std::string ("Syntax error - indentation is not right.");
}

////////////////////////////////////////////////////////////////////////////////
unsigned int Rules::getIndentation (const std::string& line)
{
  auto indent = line.find_first_not_of (" ");
  if (indent == std::string::npos)
    indent = 0;

  return indent;
}

////////////////////////////////////////////////////////////////////////////////
// Tokenize the line. This loses whitespace information and token types.
std::vector <std::string> Rules::tokenizeLine (const std::string& line)
{
  std::vector <std::string> tokens;
  std::string token;
  Lexer::Type type;
  Lexer lexer (line);
  while (lexer.token (token, type))
    tokens.push_back (token);

  return tokens;
}

////////////////////////////////////////////////////////////////////////////////
// {"one","two:"}   --> "two"
// {"one two", ":"} --> "one two"
// {"one"}          --> ""
std::string Rules::parseGroup (const std::vector <std::string>& tokens)
{
  auto count = tokens.size ();
  if (count)
  {
    auto last = tokens.back ();

    if (count >= 2 && last == ":")
      return tokens[count - 2];

    else if (count >= 1 && last[last.length () - 1] == ':')
      return last.substr (0, last.length () - 1);
  }

  return "";
}

////////////////////////////////////////////////////////////////////////////////
