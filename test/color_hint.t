#!/usr/bin/env python3

###############################################################################
#
# Copyright 2025, Gothenburg Bit Factory.
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
# https://opensource.org/license/mit
#
###############################################################################

import os
import sys
import unittest

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestColorHint(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_color_hint_overrides_config_and_works_when_piped(self):
        """:color hint should override color=off config and work with piped output"""
        # Set color to off in config file
        try:
            self.t.config("color", "off")
        except:
            pass

        # Track some time
        self.t("start 1h ago foo")
        self.t("stop")
        
        # Test with day command - output is naturally piped in test framework
        code, out, err = self.t(":color day")
        self.assertIn("\033[", out, "Expected colors with :color hint for day command")
        
        # Test with summary command
        code, out, err = self.t(":color summary")
        self.assertIn("\033[", out, "Expected colors with :color hint for summary command")
        
        # Test with tags command
        code, out, err = self.t(":color tags")
        self.assertIn("\033[", out, "Expected colors with :color hint for tags command")

    def test_nocolor_hint_disables_color(self):
        """:nocolor hint should disable color output"""
        # Set color to on in config file
        self.t.config("color", "on")
        
        # Track some time
        self.t("start 1h ago bar")
        self.t("stop")
        
        # Use :nocolor hint
        code, out, err = self.t(":nocolor day")
        
        # Check that there are no ANSI color codes
        self.assertNotIn("\033[", out, "Expected no ANSI color codes when using :nocolor hint")

    def test_no_color_by_default_when_piped(self):
        """By default, no color should be used when output is piped"""
        # Don't set any color config (should default to off for pipes)
        
        # Track some time
        self.t("start 2h ago qux")
        self.t("stop 1h ago")
        
        # Without :color hint, output should not have colors when piped
        # (test framework pipes output by default)
        code, out, err = self.t("day")
        self.assertNotIn("\033[", out, "Expected no colors by default when piped")
        
        code, out, err = self.t("summary")
        self.assertNotIn("\033[", out, "Expected no colors in summary by default when piped")


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())
