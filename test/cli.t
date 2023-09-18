#!/usr/bin/env python3

###############################################################################
#
# Copyright 2016 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# https://www.opensource.org/licenses/mit-license.php
#
###############################################################################

import json
import os
import shutil
import sys
import unittest
from datetime import datetime, timedelta

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestCLI(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_initial_call_of_timew(self):
        """Verify that calling 'timew' the first time returns exit code 0"""
        self.t.reset_env()
        shutil.rmtree(self.t.env["TIMEWARRIORDB"])

        code, out, err = self.t.runSuccess(":yes")
        self.assertIn("Welcome to Timewarrior.\n", out)

        assert os.path.isdir(self.t.env["TIMEWARRIORDB"])
        assert os.path.exists(self.t.env["TIMEWARRIORDB"])
        assert os.path.exists(os.path.join(self.t.env["TIMEWARRIORDB"], "data"))
        assert os.path.exists(os.path.join(self.t.env["TIMEWARRIORDB"], "data", "tags.data"))
        assert not os.path.exists(os.path.join(self.t.env["TIMEWARRIORDB"], "data", "undo.data"))

    def test_tag_database_is_recreated(self):
        """Verify that calling 'timew' recreates tag database"""
        now_utc = datetime.now().utcnow()

        four_hours_before_utc = now_utc - timedelta(hours=4)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} FOO".format(four_hours_before_utc, three_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} BAR".format(three_hours_before_utc, two_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} FOO".format(two_hours_before_utc, one_hour_before_utc))

        os.remove(os.path.join(self.t.env["TIMEWARRIORDB"], "data", "tags.data"))

        assert not os.path.exists(os.path.join(self.t.env["TIMEWARRIORDB"], "data", "tags.data"))

        self.t.runError("")

        assert os.path.exists(os.path.join(self.t.env["TIMEWARRIORDB"], "data", "tags.data"))

        with open(os.path.join(self.t.env["TIMEWARRIORDB"], "data", "tags.data")) as f:
            data = json.load(f)
            self.assertIn("FOO", data)
            self.assertEqual(data["FOO"]["count"], 2)
            self.assertIn("BAR", data)
            self.assertEqual(data["BAR"]["count"], 1)

    def test_TimeWarrior_without_command_without_active_time_tracking(self):
        """Call 'timew' without active time tracking"""
        code, out, err = self.t.runError()
        self.assertIn("There is no active time tracking", out)

    def test_TimeWarrior_without_command_with_active_time_tracking(self):
        """Call 'timew' with active time tracking"""
        self.t("start FOO")
        code, out, err = self.t()
        self.assertIn("Tracking FOO\n", out)

    def test_TimeWarrior_with_invalid_command(self):
        """Call a non-existing TimeWarrior command should be an error"""
        code, out, err = self.t.runError("bogus")
        self.assertIn("'bogus' is not a timew command. See 'timew help'.", err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
