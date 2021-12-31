////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016, 2018 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

#ifndef INCLUDED_EXTENSIONS
#define INCLUDED_EXTENSIONS

#include <vector>
#include <string>

class Extensions
{
public:
  Extensions () = delete;
  Extensions (const std::string&);
  Extensions (const Extensions&) = delete;
  Extensions (Extensions&&) = default;
  Extensions& operator= (const Extensions&) = delete;
  Extensions& operator= (Extensions&&) = default;

  void debug ();
  const std::vector <std::string>& all () const;
  int callExtension (const std::string&, const std::vector <std::string>&, std::vector <std::string>&) const;
  std::string dump () const;

private:
  std::vector <std::string> _scripts {};
  bool                      _debug   {false};
};

#endif
