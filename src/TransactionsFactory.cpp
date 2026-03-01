////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 - 2019, 2022 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
// https://opensource.org/license/mit
//
////////////////////////////////////////////////////////////////////////////////

#include <JSON.h>
#include <TransactionsFactory.h>
#include <memory>
#include <vector>

void TransactionsFactory::parseLine (const std::string& line)
{
  if (! line.compare (0, 4, "txn:"))
  {
    _transactions.emplace_back ();
  }
  else if (! line.compare (0, 7, "  type:"))
  {
    _type = line.substr (8, line.size ());
  }
  else if (! line.compare (0, 9, "  before:"))
  {
    _before = line.substr (10, line.size ());
  }
  else if (! line.compare (0, 8, "  after:"))
  {
    _after = line.substr (9, line.size ());
    _transactions.back ().addUndoAction (_type, _before, _after);
  }
  else
  {
    throw "Cannot handle line '" + line + "'";
  }
}

std::vector <Transaction> TransactionsFactory::get ()
{
  return _transactions;
}

void TransactionsFactory::parseJsonLine (const std::string& line)
{
  std::unique_ptr <json::value> parsed (json::parse (line));
  auto* root = dynamic_cast <json::object*> (parsed.get ());

  if (! root)
    throw std::string ("Invalid JSON transaction line: ") + line;

  auto actionsIt = root->_data.find ("actions");
  if (actionsIt == root->_data.end ())
    throw std::string ("Missing 'actions' in JSON transaction: ") + line;

  auto* actions = dynamic_cast <json::array*> (actionsIt->second);
  if (! actions)
    throw std::string ("'actions' is not an array in JSON transaction: ") + line;

  _transactions.emplace_back ();

  for (auto* item : actions->_data)
  {
    auto* action = dynamic_cast <json::object*> (item);
    if (! action)
      throw std::string ("Action is not an object in JSON transaction");

    auto typeIt = action->_data.find ("type");
    if (typeIt == action->_data.end ())
      throw std::string ("Missing 'type' in action");

    auto* typeVal = dynamic_cast <json::string*> (typeIt->second);
    if (! typeVal)
      throw std::string ("'type' is not a string in action");
    std::string type = typeVal->_data;

    std::string before;
    auto beforeIt = action->_data.find ("before");
    if (beforeIt != action->_data.end () && beforeIt->second->type () == json::j_string)
      before = json::decode (dynamic_cast <json::string*> (beforeIt->second)->_data);

    std::string after;
    auto afterIt = action->_data.find ("after");
    if (afterIt != action->_data.end () && afterIt->second->type () == json::j_string)
      after = json::decode (dynamic_cast <json::string*> (afterIt->second)->_data);

    _transactions.back ().addUndoAction (type, before, after);
  }
}
