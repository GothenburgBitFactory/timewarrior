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

#ifndef INCLUDED_JOURNAL
#define INCLUDED_JOURNAL


#include <vector>
#include <string>
#include <memory>
#include <Transaction.h>

class Journal
{
public:
  Journal() = default;

  void initialize(const std::string&);

  void startTransaction ();
  void endTransaction ();
  void recordConfigAction(const std::string&, const std::string&);
  void recordIntervalAction(const std::string&, const std::string&);

  Transaction popLastTransaction();

private:
  void recordUndoAction (const std::string &, const std::string &, const std::string &);

  std::string _location {"~/.timewarrior/data/undo.data"};
  std::shared_ptr <Transaction> _currentTransaction = nullptr;
};

#endif
