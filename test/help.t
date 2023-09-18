#!/usr/bin/env python3

###############################################################################
#
# Copyright 2019 - 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

import os
import sys
import unittest

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase
from basetest.utils import run_cmd_wait_nofail


class TestHelp(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_help_without_command_should_print_usage(self):
        """timew help without command should print usage"""
        _, out, _ = self.t("help")

        self.assertRegex(out, r"Usage: timew [--version]")

    def test_help_long_option_should_print_usage(self):
        """timew --help should print usage"""
        _, expected, _ = self.t("help")
        _, actual, _ = self.t("--help")

        self.assertEqual(actual, expected)

    def test_help_short_option_should_print_usage(self):
        """timew -h should print usage"""
        _, expected, _ = self.t("help")
        _, actual, _ = self.t("-h")

        self.assertEqual(actual, expected)

    def test_help_with_command_should_show_man_page(self):
        """timew help with command should show man page"""
        _, expected, _ = run_cmd_wait_nofail(["man", "timew-start"], env=self.t.env)
        _, actual, _ = self.t("help start")

        self.assertEqual(actual, expected)

    def test_help_with_unknown_argument_should_show_error_message(self):
        """timew help with unknown argument should show error message"""
        _, _, expected = run_cmd_wait_nofail(["man", "timew-bogus"], env=self.t.env)
        _, _, actual = self.t.runError("help bogus")

        self.assertEqual(actual, expected)

    def test_command_with_help_long_option_should_show_help_page(self):
        """timew command with --help should show help page"""
        _, expected, _ = self.t("help track")
        _, actual, _ = self.t("track --help")

        self.assertEqual(actual, expected)

    def test_command_with_help_short_option_should_show_help_page(self):
        """timew command with -h should show help page"""
        _, expected, _ = self.t("help track")
        _, actual, _ = self.t("track -h")

        self.assertEqual(actual, expected)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
