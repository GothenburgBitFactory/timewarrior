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

class TestTags(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_tags(self):
        """Test trivial tags"""
        code, out, err = self.t("tags")
        self.assertIn('No data found.', out)

    def test_tags_listed(self):
        """Test the two tags used are both listed"""
        self.t("track 20160101T0100 - 20160101T1000 foo")
        self.t("track 20160101T1000 - 20160101T1100 bar")
        code, out, err = self.t("tags")
        self.assertIn('foo', out)
        self.assertIn('bar', out)

    def test_tags_filtered(self):
        """Test that tags command filtering excludes tags that are outside the filter range"""
        self.t("track 20160101T0100 - 20160101T1000 foo")
        # self.t("track 9am - 11am bar")
        # code, out, err = self.t("tags :week")
        self.t("track 20160104T0100 - 20160104T1000 bar")
        code, out, err = self.t("tags 2016-01-02 - 2016-01-06")
        self.assertNotIn('foo', out)
        self.assertIn('bar', out)

#class TestTagFeedback(TestCase):
#    def setUp(self):
#        """Executed before each test in the class"""
#        self.t = Timew()

#    def test_verbose_new_tag(self):
#        """Test verbose feedback for new tag"""
#        code, out, err = self.t("start foo bar")
#        self.assertIn("Note: 'foo' is a new tag.", out)
#        self.assertIn("Note: 'bar' is a new tag.", out)

#    def test_verbose_repeat_tag(self):
#        """Test verbose feedback for repeat tag"""
#        self.t("track yesterday - today foo bar")
#        code, out, err = self.t("start bar baz")
#        self.assertNotIn("Note: 'bar' is a new tag.", out)
#        self.assertIn("Note: 'baz' is a new tag.", out)

#    def test_quiet_new_tag(self):
#        """Test quiet feedback for new tag"""
#        code, out, err = self.t("start foo bar :quiet")
#        self.assertNotIn("Note: 'foo' is a new tag.", out)
#        self.assertNotIn("Note: 'bar' is a new tag.", out)

#    def test_quiet_repeat_tag(self):
#        """Test quiet feedback for repeat tag"""
#        self.t("track yesterday - today foo bar")
#        code, out, err = self.t("start bar baz :quiet")
#        self.assertNotIn("Note: 'foo' is a new tag.", out)
#        self.assertNotIn("Note: 'bar' is a new tag.", out)

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
