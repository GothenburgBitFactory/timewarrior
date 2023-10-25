////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 - 2023, Shaun Ruffell, Thomas Lauf.
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

#include <AtomicFile.h>
#include <FS.h>
#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <format.h>
#include <iostream>
#include <timew.h>
#include <unistd.h>
#include <vector>

struct AtomicFile::impl
{
  using value_type = std::shared_ptr <AtomicFile::impl>;
  // Since it should be relatively small, keep the atomic files in a vector
  using atomic_files_t = std::vector <value_type>;
  using iterator = atomic_files_t::iterator;

  File temp_file;
  File real_file;

  // After the file is modified in any way, all operations should deal only with
  // the temp file until finalization.
  bool is_temp_active {false};

  explicit impl (const Path& path);
  ~impl ();

  std::string name () const;
  const std::string& path () const;
  bool exists () const;

  bool open ();
  void close ();
  size_t size () const;
  void truncate ();
  void remove ();
  void read (std::string& content);
  void read (std::vector <std::string>& lines);
  void append (const std::string& content);
  void write_raw (const std::string& content);

  void finalize ();

  static atomic_files_t::iterator find (const std::string& path) = delete;
  static atomic_files_t::iterator find (const Path& path);

  // Static members

  // If there is a problem writing to any of the temporary files, we do not want
  // any of them to be copied over the "real" file.
  static bool allow_atomics;
  static atomic_files_t atomic_files;
};

AtomicFile::impl::atomic_files_t AtomicFile::impl::atomic_files {};
bool AtomicFile::impl::allow_atomics {true};

////////////////////////////////////////////////////////////////////////////////
AtomicFile::impl::impl (const Path& path)
{
  static pid_t s_pid = ::getpid ();
  static int s_count = 0;
  std::stringstream str; 

  std::string real_path;
  if (path.is_link ())
  {
    real_path = path.realpath ();
  }
  else
  {
    real_path = path._data;
  }

  str << real_path << '.' << s_pid << '-' << ++s_count << ".tmp";
  temp_file = File (str.str ());
  real_file = File (real_path);
}

////////////////////////////////////////////////////////////////////////////////
AtomicFile::impl::~impl ()
{
  // Make sure we remove any temporary files if AtomicFile::finalize_all was
  // never called. Typically, this will happen when there are exceptions.
  try
  {
    std::remove (temp_file._data.c_str ());
  }
  catch (...)
  {
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string AtomicFile::impl::name () const
{
  return real_file.name ();
}

////////////////////////////////////////////////////////////////////////////////
const std::string& AtomicFile::impl::path () const
{
  return real_file._data;
}

////////////////////////////////////////////////////////////////////////////////
bool AtomicFile::impl::exists () const
{
  return real_file.exists ();
}

////////////////////////////////////////////////////////////////////////////////
bool AtomicFile::impl::open ()
{
  assert (!temp_file._data.empty () && !real_file._data.empty ());
  return real_file.open ();
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::close ()
{
  try
  {
    temp_file.close ();
    real_file.close ();
  }
  catch (...)
  {
    allow_atomics = false;
    throw;
  }
}

////////////////////////////////////////////////////////////////////////////////
size_t AtomicFile::impl::size () const
{
  struct stat s;
  const char *filename = (is_temp_active) ? temp_file._data.c_str () : real_file._data.c_str ();
  if (stat (filename, &s))
  {
    throw format ("stat error {1}: {2}", errno, strerror (errno));
  }
  return s.st_size;
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::truncate ()
{
  try 
  {
    temp_file.truncate ();
    is_temp_active = true;
  }
  catch (...)
  {
    allow_atomics = false;
    throw;
  }
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::remove ()
{
  try
  {
    temp_file.remove ();
    is_temp_active = true;
  }
  catch (...)
  {
    allow_atomics = false;
    throw;
  }
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::read (std::string& content)
{
  if (is_temp_active)
  {
    // Close the file before reading it in order to flush any buffers.
    temp_file.close ();
  }
  return (is_temp_active) ? temp_file.read (content) :
                            real_file.read (content);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::read (std::vector <std::string>& lines)
{
  if (is_temp_active)
  {
    // Close the file before reading it in order to flush any buffers.
    temp_file.close ();
  }
  return (is_temp_active) ? temp_file.read (lines) :
                            real_file.read (lines);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::append (const std::string& content)
{
  try
  {
    if (!is_temp_active)
    {
      is_temp_active = true;

      if (real_file.exists () && ! File::copy (real_file, temp_file))
      {
        throw format ("Failed to copy '{1}' to '{2}'",
                      real_file.name (), temp_file.name ());
      }
    }
    return temp_file.append (content);
  }
  catch (...)
  {
    allow_atomics = false;
    throw;
  }
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::write_raw (const std::string& content)
{
  try
  {
    temp_file.write_raw (content);
    is_temp_active = true;
  }
  catch (...)
  {
    allow_atomics = false;
    throw;
  }
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::impl::finalize ()
{
  if (is_temp_active && impl::allow_atomics)
  {
    if (temp_file.exists ())
    {
      debug (format ("Moving '{1}' -> '{2}'", temp_file._data, real_file._data));
      if (std::rename (temp_file._data.c_str (), real_file._data.c_str ()))
      {
        throw format("Failed copying '{1}' to '{2}'. Database corruption possible.",
            temp_file._data, real_file._data);
      }
    }
    else
    {
      debug (format ("Removing '{1}'", real_file._data));
      std::remove (real_file._data.c_str ());
    }
    is_temp_active = false;
  }
}

////////////////////////////////////////////////////////////////////////////////
AtomicFile::AtomicFile (const Path& path)
{
  auto end = impl::atomic_files.end ();
  auto it = std::find_if (impl::atomic_files.begin (), end,
                          [&path] (const impl::value_type& p)
                          {
                            return p->real_file == path;
                          });

  if (it == end)
  {
    pimpl = std::make_shared <impl> (path);
    impl::atomic_files.push_back (pimpl);
  }
  else
  {
    pimpl = *it;
  }
}

////////////////////////////////////////////////////////////////////////////////
AtomicFile::AtomicFile (std::string path) : AtomicFile (Path (path))
{
}

////////////////////////////////////////////////////////////////////////////////
AtomicFile::~AtomicFile()
{
  try
  {
    pimpl->close ();
  }
  catch (...)
  {
  }
}

AtomicFile::AtomicFile (AtomicFile&&) = default;
AtomicFile& AtomicFile::operator= (AtomicFile&&) = default;

////////////////////////////////////////////////////////////////////////////////
const std::string& AtomicFile::path () const
{
  return pimpl->path ();
}

////////////////////////////////////////////////////////////////////////////////
std::string AtomicFile::name () const
{
  return pimpl->name ();
}

////////////////////////////////////////////////////////////////////////////////
bool AtomicFile::exists () const
{
  return pimpl->exists ();
}

////////////////////////////////////////////////////////////////////////////////
bool AtomicFile::open ()
{
  return pimpl->open ();
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::close ()
{
  pimpl->close ();
}

////////////////////////////////////////////////////////////////////////////////
size_t AtomicFile::size () const
{
  return pimpl->size ();
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::truncate ()
{
  pimpl->truncate ();
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::remove ()
{
  pimpl->remove ();
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::read (std::string& content)
{
  pimpl->read (content);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::read (std::vector <std::string>& lines)
{
  pimpl->read (lines);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::append (const std::string& content)
{
  pimpl->append (content);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::write_raw (const std::string& content)
{
  pimpl->write_raw (content);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::write (const Path& path, const std::string& data)
{
  AtomicFile file (path);
  file.truncate ();
  file.write_raw (data);
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::write (const Path& path, const std::vector <std::string>& lines)
{
  AtomicFile file (path);
  file.truncate ();
  for (const auto& line : lines)
  {
    file.append (line + '\n');
  }
}

////////////////////////////////////////////////////////////////////////////////
void AtomicFile::read (const Path& path, std::string& content)
{
  AtomicFile (path).read (content);
}

void AtomicFile::read (const Path& path, std::vector <std::string>& lines)
{
  AtomicFile (path).read (lines);
}

////////////////////////////////////////////////////////////////////////////////
// finalize_all - Close / Flush all temporary files and rename to final.
void AtomicFile::finalize_all ()
{
  if (!impl::allow_atomics)
  {
    throw std::string {"Unable to update database."};
  }

  // Step 1: Close / Flush all the atomic files that may still be open. If any
  // of the files fail this step (close () will throw) then we do not want to
  // move on to step 2
  for (auto& file : impl::atomic_files)
  {
    file->close ();
  }


  sigset_t new_mask;
  sigset_t old_mask;
  sigfillset (&new_mask);

  // Step 2: Rename the temp files to the *real* file
  sigprocmask (SIG_SETMASK, &new_mask, &old_mask);
  for (auto& file : impl::atomic_files)
  {
    file->finalize ();
  }
  sigprocmask (SIG_SETMASK, &old_mask, nullptr);

  // Step 3: Cleanup any references
  impl::atomic_files_t new_atomic_files;
  for (auto& file : impl::atomic_files)
  {
    // Delete entry if we are holding the last reference
    if (file.use_count () > 1)
    {
      new_atomic_files.push_back(file);
    }
  }

  new_atomic_files.swap(impl::atomic_files);
}

////////////////////////////////////////////////////////////////////////////////
// reset - Removes all current atomic files from finalization
void AtomicFile::reset ()
{
  impl::atomic_files.clear ();
  impl::allow_atomics = true;
}
