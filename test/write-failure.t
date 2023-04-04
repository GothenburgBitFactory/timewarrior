#!/usr/bin/env python3
# write-failure-test.py - Make sure the test will fail t

import filecmp
import os
import shutil
import subprocess
import sys
import tempfile
import unittest

from basetest import Timew, TestCase
from basetest.utils import run_cmd_wait_nofail

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

class WrappedTimew(Timew):
    def __init__(self):

        try:
            self._fiu_path = subprocess.check_output("command -v fiu-run", shell=True).strip()
        except:
            raise unittest.SkipTest("fiu-run is not installed")

        super().__init__()

    def __call__(self, *args, **kwargs):
        """aka t = Timew() ; t() which is now an alias to t.runSuccess()"""
        return self.run(*args, **kwargs)

    def run(self, args="", input=None, merge_streams=False, timeout=5):
        """Invoke timew with given arguments

        If you wish to pass instructions to timew such as confirmations or other
        input via stdin, you can do so by providing an input string.
        Such as input="y\ny\n".

        If merge_streams=True stdout and stderr will be merged into stdout.

        timeout = number of seconds the test will wait for every timew call.
        Defaults to 1 second if not specified. Unit is seconds.

        Returns (exit_code, stdout, stderr) if merge_streams=False
                (exit_code, output) if merge_streams=True
        """

        # This will run timewarrior with a random chance that fputs will return
        # error 28, "No space left on device", in order to simulate running out
        # of space on the drive the database is on. It is random in order to
        # catch different places in the code that are writing as part of the
        # update (tags, undo, datafiles, configs, etc..)
        command  = [
                    self._fiu_path, "-x",
                    "-c", "enable_random name=posix/stdio/gp/fputs,probability=0.05,failinfo=28",
                    "-c", "enable_random name=posix/io/rw/write,probability=0.05,failinfo=28",
                    "-c", "enable_random name=posix/io/rw/pwrite,probability=0.05,failinfo=28",
                    "-c", "enable_random name=posix/io/rw/writev,probability=0.05,failinfo=28",
                    "-c", "enable_random name=posix/io/rw/pwritev,probability=0.05,failinfo=28",
                   ]

        command.extend(self._command[:])

        args = self._split_string_args_if_string(args)
        command.extend(args)

        return run_cmd_wait_nofail(command, input,
                                   merge_streams=merge_streams,
                                   env=self.env,
                                   timeout=timeout)


class TestWriteFailure(TestCase):

    def setUp(self):
        self._goldendir = tempfile.mkdtemp(prefix="timew_golden_")
        self.t = WrappedTimew()

    def tearDown(self):
        shutil.rmtree(self._goldendir)

    def save_database(self):
        shutil.rmtree(self._goldendir)
        shutil.copytree(self.t.datadir, self._goldendir)

    def compare_dirs(self, dir1, dir2):
        compared = filecmp.dircmp(dir1, dir2)
        if (compared.left_only or compared.right_only or
            compared.diff_files or compared.funny_files):
            return False

        for subdir in compared.common_dirs:
            if not self.compare_dirs(os.path.join(dir1, subdir),
                                     os.path.join(dir2, subdir)):
                return False

        return True

    def test_write_failures_do_not_corrupt_database(self):
        """write failures shall not corrupt the database"""

        # First prepopulate the database
        start = 10
        for x in range(start, 0, -1):
            self.t.runSuccess("track tag{0} {1}min ago".format((start + 1)-x, x))

        self.save_database()

        # Now start in with the failures and make sure the database compares
        # equally
        TAG_COUNT = 30
        error_count = 0
        success_count = 0
        MAX_ERROR_COUNT=250
        for x in range(TAG_COUNT, 0, -1):
            while True: 
                exitcode, stdout, stderr = self.t("track tag-{0} {1}s ago".format((TAG_COUNT+1)-x, x))
                if exitcode != 0:
                    error_count += 1
                    if error_count >= MAX_ERROR_COUNT:
                        break
                    
                    if not self.compare_dirs(self.t.datadir, self._goldendir):
                        filecmp.dircmp(self.t.datadir, self._goldendir).report_full_closure()
                        self.fail("{0} not equal to {1}".format(self.t.datadir, self._goldendir))
                else:
                    success_count += 1
                    break

            if error_count >= MAX_ERROR_COUNT:
                break

            self.save_database()

        self.t.runSuccess("export")
        print("Test pass. Timew returned an error on {0} runs and none on {1} runs.".format(error_count, success_count))

if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
    #unittest.main()
