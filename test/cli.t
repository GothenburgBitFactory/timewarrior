#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2006 - 2018, Paul Beckingham, Federico Hernandez.
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
# http://www.opensource.org/licenses/mit-license.php
#
###############################################################################

import sys
import os
import shutil
import unittest
from datetime import datetime
# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase

# Test methods available:
#     self.assertEqual(a, b)
#     self.assertNotEqual(a, b)
#     self.assertTrue(x)
#     self.assertFalse(x)
#     self.assertIs(a, b)
#     self.assertIsNot(substring, text)
#     self.assertIsNone(x)
#     self.assertIsNotNone(x)
#     self.assertIn(substring, text)
#     self.assertNotIn(substring, text
#     self.assertRaises(e)
#     self.assertRegexpMatches(text, pattern)
#     self.assertNotRegexpMatches(text, pattern)
#     self.tap("")


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

# vim: ai sts=4 et sw=4 ft=python
