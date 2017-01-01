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

class TestDelete(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_delete_open(self):
        """Delete a single open interval"""
        self.t("track :yesterday foo")
        self.t("start today bar")

        code, out, err = self.t("delete @1")
        self.assertEqual(out, 'Deleted @1\n')

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_delete_closed(self):
        """Delete a single closed interval"""
        self.t("track :yesterday foo")
        self.t("start today bar")
        code, out, err = self.t("delete @2")
        self.assertEqual(out, 'Deleted @2\n')

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertFalse('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'bar')

    def test_delete_multiple(self):
        """Delete a mix of open/closed intervals"""
        self.t("track :yesterday foo")
        self.t("start today bar")
        code, out, err = self.t("delete @1 @2")
        self.assertIn('Deleted @1', out)
        self.assertIn('Deleted @2', out)

        j = self.t.export()
        self.assertEqual(len(j), 0)


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
