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

class TestDOM(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_dom_unrecognized(self):
        """Test dom.NOPE which is unrecognized"""
        code, out, err = self.t.runError("get dom.NOPE")
        self.assertIn("DOM reference 'dom.NOPE' is not valid.", err)

    def test_dom_active_active(self):
        """Test dom.active with and with an active interval"""
        self.t("start foo")
        code, out, err = self.t("get dom.active")
        self.assertEqual('1\n', out)

    def test_dom_active_inactive(self):
        """Test dom.active with and without an active interval"""
        code, out, err = self.t("get dom.active")
        self.assertEqual('0\n', out)

    def test_dom_active_tag_count_inactive(self):
        """Test dom.active.tag.count with no active track"""
        code, out, err = self.t("get dom.active.tag.count")
        self.assertEqual('0\n', out)

    def test_dom_active_tag_count_zero(self):
        """Test dom.active.tag.count with zero tags"""
        self.t("start")
        code, out, err = self.t("get dom.active.tag.count")
        self.assertEqual('0\n', out)

    def test_dom_active_tag_count_two(self):
        """Test dom.active.tag.count with two tags"""
        self.t("start one two")
        code, out, err = self.t("get dom.active.tag.count")
        self.assertEqual('2\n', out)

    def test_dom_active_tag_N_none(self):
        """Test dom.active.tag.N with no active track"""
        code, out, err = self.t.runError("get dom.active.tag.1")
        self.assertIn("DOM reference 'dom.active.tag.1' is not valid.", err)

    def test_dom_active_tag_N_zero(self):
        """Test dom.active.tag.N with zero tags"""
        self.t("start")
        code, out, err = self.t.runError("get dom.active.tag.1")
        self.assertIn("DOM reference 'dom.active.tag.1' is not valid.", err)

    def test_dom_active_tag_N_two(self):
        """Test dom.active.tag.N with two tags"""
        self.t("start one two")
        code, out, err = self.t("get dom.active.tag.2")
        self.assertEqual('two\n', out)

    def test_dom_active_start_inactive(self):
        """Test dom.active.start with no active track"""
        code, out, err = self.t.runError("get dom.active.start")
        self.assertIn("DOM reference 'dom.active.start' is not valid.", err)

    def test_dom_active_start_active(self):
        """Test dom.active.start with active track"""
        self.t("start one two")
        code, out, err = self.t("get dom.active.start")
        self.assertRegexpMatches(out, r'\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}')

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
