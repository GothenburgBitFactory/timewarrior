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
#include <TempDir.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <test.h>
#include <unistd.h>

#ifdef FIU_ENABLE

#include <fiu.h>
#include <fiu-control.h>

// This class is a helper to make sure we turn off libfiu after the test so that
// we can properly write to stdout / stderr.
class FIU
{
public:
  FIU()
  {
    enable();
  }

  FIU(const FIU&) = delete;
  FIU& operator= (const FIU&) = delete;

  ~FIU()
  {
    disable ();
    std::cout << cbuffer.str();
    std::stringstream().swap(cbuffer);
  }

  void enable ()
  {
    for (auto test_point : test_points)
    {
      fiu_enable_external(test_point, 1, NULL, 0, fiu_cb);
    }
  }

  void disable ()
  {
    for (auto test_point : test_points)
    {
      fiu_disable(test_point);
    }
  }

private:
  static const std::vector <const char*> test_points;
  static std::stringstream cbuffer;

  static int external_cb_was_called;

  static int fiu_cb(const char *name, int *failnum,
                    void **failinfo, unsigned int *flags)
  {
    (void)name;
    (void)flags;
    external_cb_was_called++;

    // For debugging the tests themselves...
    // cbuffer << "fiu_cb called for " << name << '\n';

    *failinfo = (void *) EIO;

    return *failnum;
  }
};

const std::vector <const char*> FIU::test_points {
  "posix/stdio/gp/fputs",
  "posix/io/rw/write",
};

std::stringstream FIU::cbuffer;
int FIU::external_cb_was_called = 0;

//////////////////////////////////////////////////////////////////////////////
// Since AtomicFile is primarily for keeping the database consistent in the
// presence of filesystem errors, these tests use libfiu to ensure that
// AtomicFile behaves as intended when the underlying system calls fail.
int fiu_test (UnitTest& t)
{
  TempDir tempDir;
  fiu_init (0);

  try { FIU fiu; AtomicFile ("test.txt").write_raw("This is  test"); t.fail ("AtomicFileTest: AtomicFile::write_raw throws on error"); }
  catch (...) {                                                      t.pass ("AtomicFileTest: AtomicFile::write_raw throws on error"); }  

  try { FIU fiu; AtomicFile::finalize_all ();                        t.fail ("AtomicFileTest: AtomicFile::finalize_all() throws on error"); }
  catch (...) {                                                      t.pass ("AtomicFileTest: AtomicFile::finalize_all() throws on error"); }

  try { FIU fiu; AtomicFile::reset (); AtomicFile::finalize_all ();  t.pass ("AtomicFileTest: AtomicFile::reset clears failure state"); }
  catch (...) {                                                      t.fail ("AtomicFileTest: AtomicFile::reset clears failure state"); }

  File::write ("test.txt", "line1\n");
  {
    AtomicFile file ("test.txt");
    try { FIU fiu; file.append ("append1\n");                        t.fail ("AtomicFileTest: AtomicFile::append throws on error"); }
    catch (...) {                                                    t.pass ("AtomicFileTest: AtomicFile::append throws on error"); }

    std::string contents {"should-not-see-this"};
    file.read (contents);
    t.is (contents, "", "AtomicFile::append did not partially fill the file.");
  }

  try { FIU fiu; AtomicFile::finalize_all ();                       t.fail ("AtomicFileTest: AtomicFile::append failures prevent finalization"); }
  catch (...) {                                                     t.pass ("AtomicFileTest: AtomicFile::append failures prevent finalization"); }

  return 0;
}
#else

#define fiu_init(flags) 0
#define fiu_fail(name) 0
#define fiu_failinfo() NULL
#define fiu_do_on(name, action)
#define fiu_exit_on(name)
#define fiu_return_on(name, retval)

int fiu_test (UnitTest& t)
{
  t.skip ("AtomicFileTest: AtomicFile::write_raw throws on error");
  t.skip ("AtomicFileTest: AtomicFile::finalize_all() throws on error");
  t.skip ("AtomicFileTest: AtomicFile::reset clears failure state");
  t.skip ("AtomicFileTest: AtomicFile::append throws on error");
  t.skip ("AtomicFileTest: AtomicFile::append did not partially fill the file.");
  t.skip ("AtomicFileTest: AtomicFile::append failures prevent finalization");
  return 0;
}
#endif // FIU_ENABLE

//////////////////////////////////////////////////////////////////////////////
int test_symlink (UnitTest& t)
{
  Path link ("link");
  Path target ("target");
  File(target).write_raw("empty\n");

  const std::string write_contents = "test file contents\n";

  if (::symlink (target._data.c_str(), link._data.c_str()) != 0)
  {
    throw std::runtime_error("Failed to create link in test_symlink");
  }

  {
    AtomicFile file(link);
    file.write_raw(write_contents);
  }
  AtomicFile::finalize_all ();


  // After finalizing we should be able read the data that we wrote through the
  // target
  std::string read_contents;
  AtomicFile::read (target, read_contents);
  t.is(read_contents, write_contents, "AtomicFileTest: writes to symlinks end up in target");
  t.is (link.is_link (), true, "AtomicFileTest: shall maintain symlink");

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
int test (UnitTest& t)
{
  std::string test_name;

  // This will create and change to a temporary directory that will be cleaned
  // up when the destructor is run
  TempDir tempDir;

  std::string goldenText = "1\n";
  std::string contents;
  std::string expected;

  Path firstFilename ("test-1.txt");
  Path secondFilename ("test-2.txt");

  {
    AtomicFile file (firstFilename);
    file.write_raw (goldenText);
    file.close ();
  }

  t.is (firstFilename.exists (), false, "AtomicFileTest: Shall not exists before finalize");
  AtomicFile::finalize_all ();
  t.is (firstFilename.exists (), true, "AtomicFileTest: Shall exists after finalize");
  File::read(firstFilename, contents);
  t.is (contents == goldenText, true, "AtomicFileTest: Shall have the correct data");

  tempDir.clear ();

  {
    AtomicFile first (firstFilename);
    AtomicFile second (secondFilename);

    first.write_raw ("first\n");
    second.write_raw ("second\n");
    first.close ();
    second.close ();
  }

  t.is (firstFilename.exists () || secondFilename.exists (), false, "AtomicFileTest: Neither shall exist before finalize");

  AtomicFile::finalize_all ();

  t.is (firstFilename.exists () && secondFilename.exists (), true, "AtomicFileTest: Both shall exists after finalize");

  File::read(firstFilename, contents);
  test_name = "AtomicFileTest: First file shall contain the correct data";
  expected = "first\n";
  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }

  File::read(secondFilename, contents);
  test_name = "AtomicFileTest: Second file shall contain the correct data";
  if (contents == std::string("second\n"))
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected 'second\n' read '" + contents + "'"));
  }

  // Make sure appending works

  test_name = "AtomicFileTest: Appending does not update original before finalize";
  expected = "first\n";

  {
    AtomicFile first (firstFilename);

    first.append("append 1\n");
    first.append("append 2\n");
    first.close ();
  }

  File::read (firstFilename, contents);
  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }

  test_name = "AtomicFileTest: Finalizing updates the appended data";
  expected = "first\nappend 1\nappend 2\n";
  AtomicFile::finalize_all ();
  File::read (firstFilename, contents);
  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }

  test_name = "AtomicFileTest: Read from Atomicfile";
  // We do not want to update the expected
  {
    AtomicFile::read (firstFilename, contents);
  }

  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }

  // If we read from an atomic file before finalizing, we should get the data
  // that was written to the temporary file and not the 'real' file.

  test_name = "AtomicFileTest: Read from Atomicfile should read unfinalized data";
  expected += "expected\n";
  AtomicFile::write (firstFilename, expected);
  AtomicFile::read (firstFilename, contents);
  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }

  AtomicFile::finalize_all ();
  {
    AtomicFile first (firstFilename);
    AtomicFile second (firstFilename);

    first.write_raw("first\n");
    second.append ("second\n");

    AtomicFile::finalize_all ();

    first.append ("third\n");
    second.append ("fourth\n");
    first.append ("fifth\n");

  }

  test_name = "AtomicFileTest: Two AtomicFiles should access same temp file (part 1)";

  // The atomic files, which were closed above should return all the strings
  // written since it was not yet finalized.

  expected = "first\nsecond\nthird\nfourth\nfifth\n";
  AtomicFile::read (firstFilename, contents);
  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }

  // But since the file was not yet finalized, the "real" file should only
  // contain the data present since before the finalize_all () call.

  test_name = "AtomicFileTest: Two AtomicFiles should access same temp file (part 2)";
  expected = "first\nsecond\n";
  File::read (firstFilename, contents);
  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }

  // After we finalize the data, the AtomicFile and the File should now both
  // return the same information

  test_name = "AtomicFileTest: Two AtomicFiles should access same temp file (part 3)";
  AtomicFile::finalize_all ();
  File::read (firstFilename, expected);
  AtomicFile::read (firstFilename, contents);
  if (contents == expected)
  {
    t.pass (test_name);
  }
  else
  {
    t.fail (test_name);
    t.diag (std::string ("Expected '" + expected + "' read '" + contents + "'"));
  }
  AtomicFile::reset ();

  {
    tempDir.clear ();
    Path test("test");
    AtomicFile file(test);
    file.truncate ();
    assert (! test.exists ());
    AtomicFile::finalize_all ();
    assert (test.exists ());
    file.remove ();
    t.is (test.exists (), true, "AtomicFileTest: File not removed before finalize");
    AtomicFile::finalize_all ();
    t.is (test.exists (), false, "AtomicFileTest: File is removed after finalize");
  }

  tempDir.clear();
  test_symlink(t);

  return 0;
}

int main (int, char**)
{
  UnitTest t (22);
  try
  {
    int ret = test (t);
    int fiu_ret = fiu_test (t);
    return (ret == 0) ? fiu_ret : ret;
  }
  catch (const std::string& error)
  {
    std::cerr << "Test threw exception: " << error << '\n';
  }
  catch (...)
  {
    std::cerr << "Uncaught exception.\n";
  }
}

////////////////////////////////////////////////////////////////////////////////

