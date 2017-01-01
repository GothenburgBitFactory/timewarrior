#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2006 - 2017, Paul Beckingham, Federico Hernandez.
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

class TestStart(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_start(self):
        """Test trivial start"""
        self.t("start")
        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertFalse('end' in j[0])

    def test_timed_start(self):
        """Test timed start"""
        self.t("start 10:30am")
        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertFalse('end' in j[0])

    def test_start_with_open_interval(self):
        """Test start with already open interval, which should be auto-stopped"""
        self.t("start 2016-01-01T00:00:00 foo")
        code, out, err = self.t("start bar")
        self.assertIn("Recorded foo", out)
        self.assertIn("Tracking bar", out)

    def test_start_with_open_interval_timed(self):
        """Test timed start with already open interval, which should be auto-stopped
        at given datetime"""
        self.t("start 2016-01-01T00:00:00 foo")
        code, out, err = self.t("start 2016-01-01T01:00:00 bar")
        self.assertIn("Recorded foo", out)
        self.assertIn("Tracking bar", out)

        self.assertIn("Ended              01:00:00",out)

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
