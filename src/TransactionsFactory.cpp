////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#include <vector>
#include <TransactionsFactory.h>

void TransactionsFactory::parseLine (const std::string& line)
{
  if (!line.compare(0, 4, "txn:"))
  {
    _transactions.emplace_back ();
  }
  else if (!line.compare(0, 7, "  type:"))
  {
    _type = line.substr (8, line.size ());
  }
  else if (!line.compare(0, 9, "  before:"))
  {
    _before = line.substr (10, line.size ());
  }
  else if (!line.compare(0, 8, "  after:"))
  {
    _after = line.substr (9, line.size ());
    _transactions.back ().addUndoAction (_type, _before, _after);
  }
  else
  {
    throw "Cannot handle line '" + line + "'";
  }
}

std::vector<Transaction> TransactionsFactory::get ()
{
  return _transactions;
}
