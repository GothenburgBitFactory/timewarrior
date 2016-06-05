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

class TestTag(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_add_tag_to_open_interval(self):
        """Add a tag to an open interval"""
        self.t("start 30min ago")
        code, out, err = self.t("tag @1 foo")
        self.assertRegexpMatches(out, 'Added: foo$')

    def test_add_tag_to_closed_interval(self):
        """Add a tag to an closed interval"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t("tag @1 foo")
        self.assertRegexpMatches(out, 'Added: foo$')

    def test_add_tags_to_open_interval(self):
        """Add tags to an open interval"""
        self.t("start 30min ago")
        code, out, err = self.t("tag @1 foo bar")
        self.assertRegexpMatches(out, 'Added: foo bar$')

    def test_add_tags_to_closed_interval(self):
        """Add tags to an closed interval"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t("tag @1 foo bar")
        self.assertRegexpMatches(out, 'Added: foo bar$')

    def test_add_tag_to_multiple_intervals(self):
        """Add a tag to multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00")
        code, out, err = self.t("tag @1 @2 foo")
        self.assertRegexpMatches(out, 'Added: foo\nAdded: foo$')

    def test_add_tags_to_multiple_intervals(self):
        """Add tags to multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00")
        code, out, err = self.t("tag @1 @2 foo bar")
        self.assertRegexpMatches(out, 'Added: foo bar\nAdded: foo bar$')

class TestUntag(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_remove_tag_from_open_interval(self):
        """Remove a tag from an open interval"""
        self.t("start 30min ago foo bar baz")
        code, out, err = self.t("untag @1 foo")
        self.assertRegexpMatches(out, 'Removed: foo$')

    def test_remove_tag_from_closed_interval(self):
        """Remove a tag from an closed interval"""
        self.t("track yesterday for 1hour foo bar baz")
        code, out, err = self.t("untag @1 foo")
        self.assertRegexpMatches(out, 'Removed: foo$')

    def test_remove_tags_from_open_interval(self):
        """Remove tags from an open interval"""
        self.t("start 30min ago foo bar baz")
        code, out, err = self.t("untag @1 foo bar")
        self.assertRegexpMatches(out, 'Removed: foo bar$')

    def test_remove_tags_from_closed_interval(self):
        """Remove tags from an closed interval"""
        self.t("track yesterday for 1hour foo bar baz")
        code, out, err = self.t("untag @1 foo bar")
        self.assertRegexpMatches(out, 'Removed: foo bar$')

    def test_remove_tag_from_multiple_intervals(self):
        """Remove a tag from multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar baz")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00 foo bar baz")
        code, out, err = self.t("untag @1 @2 foo")
        self.assertRegexpMatches(out, 'Removed: foo\nRemoved: foo$')

    def test_remove_tags_from_multiple_intervals(self):
        """Remove tags from multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar baz")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00 foo bar baz")
        code, out, err = self.t("untag @1 @2 foo bar")
        self.assertRegexpMatches(out, 'Removed: foo bar\nRemoved: foo bar$')

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
