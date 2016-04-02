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
const std::string A2::attribute (const std::string& name) const
{
  // Prevent autovivification.
  auto i = _attributes.find (name);
  if (i != _attributes.end ())
    return i->second;

  return "";
}

////////////////////////////////////////////////////////////////////////////////
const std::string A2::getToken () const
{
  auto i = _attributes.find ("canonical");
  if (i == _attributes.end ())
    i = _attributes.find ("raw");

  return i->second;
}

////////////////////////////////////////////////////////////////////////////////
const std::string A2::dump () const
{
  auto output = Lexer::typeToString (_lextype);

  // Dump attributes.
  std::string atts;
  for (const auto& a : _attributes)
    atts += a.first + "='\033[33m" + a.second + "\033[0m' ";

  // Dump tags.
  std::string tags;
  for (const auto& tag : _tags)
  {
    tags += "\033[32m" + tag + "\033[0m ";
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
const std::string CLI::dump (const std::string& title) const
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

  out << "\n";

  if (_args.size ())
  {
    out << "  _args\n";
    for (const auto& a : _args)
      out << "    " << a.dump () << "\n";
  }

  return out.str ();
}

////////////////////////////////////////////////////////////////////////////////
