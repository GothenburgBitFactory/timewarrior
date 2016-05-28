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

class TestGaps(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_gaps(self):
        """Test trivial track"""
        code, out, err = self.t("gaps")
        self.assertRegexpMatches(out, r'\s{30}23:59:59')

    def test_single_unobstructed_interval(self):
        """Add one interval and export it as-is"""
        self.t("track 20160527T080000 - 20160527T200000 foo")

        code, out, err = self.t("gaps 2016-05-27 - 2016-05-28")
        self.assertRegexpMatches(out, r'\s{30}11:59:59')

    def test_single_unobstructed_interval_with_exclusions(self):
        """Add one interval, with exclusions"""
        self.t.config("exclusions.monday",    "<9:00 >18:00")
        self.t.config("exclusions.tuesday",   "<9:00 >18:00")
        self.t.config("exclusions.wednesday", "<9:00 >18:00")
        self.t.config("exclusions.thursday",  "<9:00 >18:00")
        self.t.config("exclusions.friday",    "<9:00 >18:00")
        self.t.config("exclusions.saturday",  "<9:00 >18:00")
        self.t.config("exclusions.sunday",    "<9:00 >18:00")
        self.t("track 20160527T100000 - 20160527T140000 foo")

        code, out, err = self.t("gaps 2016-05-27 - 2016-05-28")
        self.assertRegexpMatches(out, r'\s{30}5:00:00')

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
