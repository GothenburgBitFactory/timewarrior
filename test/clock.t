#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2006 - 2016, Paul Beckingham, Federico Hernandez.
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

class TestClock(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_stop_nothing(self):
        """Verify that stopping an unstarted interval does nothing"""
        code, out, err = self.t("stop")
        self.assertIn("There is no active time tracking.", out)

    def test_start_new(self):
        """Verify that 'start' creates an open interval"""
        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

        code, out, err = self.t("start tag1 tag2")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t("export")
        self.assertIn('"start":', out)
        self.assertNotIn('"end":', out)
        self.assertIn('"tags":["tag1","tag2"]', out)

    def test_start_stop(self):
        """Verify that start/stop creates and closes an interval"""
        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

        code, out, err = self.t("start tag1 tag2")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t("stop")
        self.assertIn("Recorded tag1 tag2\n", out)

        code, out, err = self.t("export")
        self.assertIn('"start":', out)
        self.assertIn('"end":', out)
        self.assertIn('"tags":["tag1","tag2"]', out)

        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

    def test_start_additional(self):
        """Verify that 'start' closes an open interval and starts a new one"""
        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

        code, out, err = self.t("start tag1 tag2")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t("start tag3")
        self.assertIn("Recorded tag1 tag2\n", out)
        self.assertIn("Tracking tag3\n", out)

        code, out, err = self.t("stop")
        self.assertIn("Recorded tag3\n", out)

        code, out, err = self.t("export")
        self.assertIn('"start":', out)
        self.assertIn('"end":', out)
        self.assertIn('"tags":["tag1","tag2"]', out)
        self.assertIn('"tags":["tag3"]', out)

        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

    def test_start_subtract(self):
        """Verify that starting multiple tags and stopping one leaves an open interval"""
        code, out, err = self.t("start tag1 tag2 tag3")
        self.assertIn("Tracking tag1 tag2 tag3\n", out)

        code, out, err = self.t("stop tag1")
        self.assertIn("Recorded tag1 tag2 tag3\n", out)
        self.assertIn("Tracking tag2 tag3\n", out)

        code, out, err = self.t("stop")
        self.assertIn("Recorded tag2 tag3\n", out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
