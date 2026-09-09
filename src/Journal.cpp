////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <AtomicFile.h>
#include <Journal.h>
#include <TransactionsFactory.h>
#include <cassert>
#include <iostream>
#include <timew.h>

////////////////////////////////////////////////////////////////////////////////

bool Journal::enabled () const
{
  return _size != 0;
}

////////////////////////////////////////////////////////////////////////////////

static std::vector <Transaction> loadJournal (AtomicFile& undo)
{
  std::vector <std::string> read_lines;
  undo.read (read_lines);
  undo.close ();

  // Find first non-empty line to detect format
  std::string firstLine;
  for (auto& line : read_lines)
  {
    if (! line.empty ())
    {
      firstLine = line;
      break;
    }
  }

  if (firstLine.empty ())
    return {};

  TransactionsFactory transactionsFactory;

  if (firstLine[0] == '{')
  {
    // JSON format: each non-empty line is a complete transaction
    for (auto& line : read_lines)
    {
      if (! line.empty ())
        transactionsFactory.parseJsonLine (line);
    }
  }
  else
  {
    // Legacy text format: multiple lines per transaction
    for (auto& line : read_lines)
      transactionsFactory.parseLine (line);
  }

  return transactionsFactory.get ();
}

////////////////////////////////////////////////////////////////////////////////
void Journal::initialize (const std::string& location, int size, const std::string& format)
{
  _location = location;
  _size = size;
  _useJsonFormat = (format == "json");

  if (! enabled ())
  {
    AtomicFile undo (_location);
    if (undo.exists () && undo.size () > 0)
    {
      undo.remove ();
    }
    return;
  }

  // Detect format of existing file and warn or migrate as needed
  AtomicFile undo (_location);
  if (undo.exists () && undo.size () > 0)
  {
    std::vector <std::string> lines;
    undo.read (lines);
    undo.close ();

    // Find first non-empty line to detect format
    std::string firstLine;
    for (auto& line : lines)
    {
      if (! line.empty ())
      {
        firstLine = line;
        break;
      }
    }

    bool isLegacyFormat = (! firstLine.empty () && firstLine.compare (0, 4, "txn:") == 0);

    if (isLegacyFormat)
    {
      if (_useJsonFormat)
      {
        // Migrate legacy format to JSON
        std::cout << "Migrating undo.data to JSON format." << std::endl;

        TransactionsFactory factory;
        for (auto& line : lines)
          factory.parseLine (line);

        std::vector <Transaction> transactions = factory.get ();

        undo.truncate ();
        for (auto& txn : transactions)
          undo.append (txn.toJson ());
      }
      else
      {
        // Warn about the new format being available
        warn ("undo.data uses a legacy format. Set 'journal.format=json' in your configuration to switch to the new JSON format and trigger automatic migration.");
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
void Journal::startTransaction ()
{
  if (enabled ())
  {
    if (_currentTransaction != nullptr)
    {
      throw "Subsequent call to start transaction";
    }

    _currentTransaction = std::make_shared <Transaction> ();
  }
}

////////////////////////////////////////////////////////////////////////////////
void Journal::endTransaction ()
{
  if (! enabled ())
  {
    assert (_currentTransaction == nullptr);
    return;
  }

  if (_currentTransaction == nullptr)
  {
    throw "Call to end non-existent transaction";
  }

  AtomicFile undo (_location);

  if (_size > 1)
  {
    std::vector <Transaction> transactions = loadJournal (undo);

    unsigned int toCopy = _size - 1;
    auto it = transactions.cbegin ();
    auto end = transactions.cend ();

    if (transactions.size () > toCopy)
    {
      it += transactions.size () - toCopy;
    }

    undo.truncate ();
    for (; it != end; ++it)
    {
      undo.append (_useJsonFormat ? it->toJson () : it->toString ());
    }
  }
  else if (_size == 1)
  {
    undo.truncate ();
  }

  undo.append (_useJsonFormat ? _currentTransaction->toJson () : _currentTransaction->toString ());
  _currentTransaction.reset ();
}

////////////////////////////////////////////////////////////////////////////////
void Journal::recordConfigAction (const std::string& before, const std::string& after)
{
  recordUndoAction ("config", before, after);
}

////////////////////////////////////////////////////////////////////////////////
void Journal::recordIntervalAction (const std::string&  before, const std::string&  after)
{
  recordUndoAction ("interval", before, after);
}

////////////////////////////////////////////////////////////////////////////////
// Record undoable actions. There are several types:
//   interval    changes to stored intervals
//   config      changes to configuration
//
// Actions are only recorded if a transaction is open
//
void Journal::recordUndoAction (
  const std::string& type,
  const std::string& before,
  const std::string& after)
{
  if (enabled () && _currentTransaction != nullptr)
  {
    _currentTransaction->addUndoAction (type, before, after);
  }
}

////////////////////////////////////////////////////////////////////////////////
Transaction Journal::popLastTransaction ()
{
  if (! enabled ())
  {
    return Transaction {};
  }

  AtomicFile undo (_location);
  std::vector <Transaction> transactions = loadJournal (undo);

  if (transactions.empty ())
  {
    return Transaction {};
  }

  Transaction last = transactions.back ();
  transactions.pop_back ();

  if (transactions.empty ())
  {
    undo.close ();
    undo.remove ();
  }
  else
  {
    undo.open ();
    undo.truncate ();

    for (auto& transaction : transactions)
    {
      undo.append (_useJsonFormat ? transaction.toJson () : transaction.toString ());
    }

    undo.close ();
  }

  return last;
}
