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

#ifndef TIMEW_TEMP_DIR
#define TIMEW_TEMP_DIR

#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <vector>

#include <FS.h>

class TempDir
{
public:
  TempDir ();
  ~TempDir ();

  bool is_empty () const;
  void clear ();
  std::vector <std::string> file_names () const;

  const std::string& name () const { return tmpName; };
  operator const std::string& () const { return name (); }

private:
  std::string tmpName {};
  std::string oldDir {};
};

////////////////////////////////////////////////////////////////////////////////
TempDir::TempDir ()
{
  char template_name[PATH_MAX] = {'\0',};

  oldDir = Directory::cwd ();

  strncpy (template_name, (oldDir + "/timew_test_XXXXXX").c_str (), sizeof (template_name) - 1);
  const char *cwd = ::mkdtemp (template_name);
  if (cwd == nullptr)
  {
    throw std::string ("Failed to create temp directory");
  }

  tmpName = cwd;

  if (::chdir (tmpName.c_str ()))
  {
    throw std::string ("Failed to change to temporary directory");
  }
}

////////////////////////////////////////////////////////////////////////////////
TempDir::~TempDir ()
{
  try
  {
    clear ();
    ::chdir (oldDir.c_str ());

    if (!Directory(tmpName).remove ())
    {
      std::cerr << "Failed to remove temp dir " << tmpName << '\n';
    }
  }
  catch (...)
  {
    std::cerr << "Unhandled exception in " << __func__ << '\n';
  }
}

////////////////////////////////////////////////////////////////////////////////
bool TempDir::is_empty () const
{
  return file_names().empty ();
}

////////////////////////////////////////////////////////////////////////////////
std::vector <std::string> TempDir::file_names () const
{
  return Path (tmpName).glob ("*");
}

////////////////////////////////////////////////////////////////////////////////
void TempDir::clear ()
{
  for (const auto& file : file_names ())
  {
    File::remove (file);
  }
}

#endif // TIMEW_TEMP_DIR
