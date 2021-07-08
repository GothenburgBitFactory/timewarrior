////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 - 2021, Shaun Ruffell, Thomas Lauf.
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
#ifndef INCLUDED_ATOMICFILE
#define INCLUDED_ATOMICFILE

#include <memory>
#include <vector>

class Path;

class AtomicFile
{
public:
  AtomicFile (const Path& path);
  AtomicFile (std::string path);
  AtomicFile (const AtomicFile&) = delete;
  AtomicFile (AtomicFile&&);
  AtomicFile& operator= (const AtomicFile&) = delete;
  AtomicFile& operator= (AtomicFile&&);
  ~AtomicFile ();

  std::string name () const;
  const std::string& path () const;
  bool exists () const;

  bool open ();
  void close ();
  void remove ();
  void truncate ();
  size_t size () const;
  void read (std::string& content);
  void read (std::vector <std::string>& lines);
  void append (const std::string& content);
  void write_raw (const std::string& content);

  static void append (const Path& path, const std::string& data);

  static void write (const Path& path, const std::string& data);
  static void write (const Path& path, const std::vector <std::string>& lines);

  static void read (const Path& path, std::string& content);
  static void read (const Path& path, std::vector <std::string>& lines);

  static void finalize_all ();
  static void reset ();

private:
  struct impl;
  std::shared_ptr <impl> pimpl;
};

#endif // INCLUDED_ATOMICFILE
