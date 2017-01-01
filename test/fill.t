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

class TestFillHint(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_unfilled_track_in_gap(self):
        """Add closed interval into a gap without fill"""
        self.t("track 20160709T050000Z - 20160709T060000Z one")
        self.t("track 20160709T090000Z - 20160709T100000Z three")
        code, out, err = self.t("track 20160709T070000Z - 20160709T080000Z two")
        self.assertNotIn('Backfilled to ', out)
        self.assertNotIn('Filled to ', out)

        j = self.t.export()
        self.assertEqual(len(j), 3)
        self.assertEqual(j[0]['start'], '20160709T050000Z')
        self.assertEqual(j[0]['end'],   '20160709T060000Z')
        self.assertEqual(j[0]['tags'][0], 'one')
        self.assertEqual(j[1]['start'], '20160709T070000Z')
        self.assertEqual(j[1]['end'],   '20160709T080000Z')
        self.assertEqual(j[1]['tags'][0], 'two')
        self.assertEqual(j[2]['start'], '20160709T090000Z')
        self.assertEqual(j[2]['end'],   '20160709T100000Z')
        self.assertEqual(j[2]['tags'][0], 'three')

    def test_filled_track_in_gap(self):
        """Add closed interval into a gap with fill"""
        self.t("track 20160709T050000Z - 20160709T060000Z one")
        self.t("track 20160709T090000Z - 20160709T100000Z three")
        code, out, err = self.t("track 20160709T070000Z - 20160709T080000Z two :fill")
        self.assertIn('Backfilled to ', out)
        self.assertIn('Filled to ', out)

        j = self.t.export()
        self.assertEqual(len(j), 3)
        self.assertEqual(j[0]['start'], '20160709T050000Z')
        self.assertEqual(j[0]['end'],   '20160709T060000Z')
        self.assertEqual(j[0]['tags'][0], 'one')
        self.assertEqual(j[1]['start'], '20160709T060000Z')
        self.assertEqual(j[1]['end'],   '20160709T090000Z')
        self.assertEqual(j[1]['tags'][0], 'two')
        self.assertEqual(j[2]['start'], '20160709T090000Z')
        self.assertEqual(j[2]['end'],   '20160709T100000Z')
        self.assertEqual(j[2]['tags'][0], 'three')

    def test_filled_start(self):
        """Add an open interval with fill"""
        self.t("track 20160710T100000Z - 20160710T110000Z one")
        code, out, err = self.t("start 20160710T113000Z two :fill")
        self.assertIn('Backfilled to ', out)
        self.assertNotIn('Filled to ', out)

        j = self.t.export()
        self.assertEqual(len(j), 2)
        self.assertEqual(j[0]['start'], '20160710T100000Z')
        self.assertEqual(j[0]['end'],   '20160710T110000Z')
        self.assertEqual(j[0]['tags'][0], 'one')
        self.assertEqual(j[1]['start'], '20160710T110000Z')
        self.assertTrue('end' not in j[1])
        self.assertEqual(j[1]['tags'][0], 'two')

#class TestFillCommand(TestCase):
#    def setUp(self):
#        """Executed before each test in the class"""
#        self.t = Timew()
#
#    def test_fill_command(self):
#        """Create gaps, then fill them"""
#        self.t("track 20160724T090000 - 20160724T100000 foo")
#        # 11:12 gap here.
#        self.t("track 20160724T101112 - 20160724T101213 bar")
#        # 47:47 gap here.
#        self.t("track 20160724T110000 - 20160724T120000 baz")
#
#        # Eliminate gaps.
#        code, out, err = self.t("fill @2")
#        self.assertIn('Backfilled @2 to 2016-07-24T10:00:00', out)
#        self.assertIn('Filled @2 to 2016-07-24T11:00:00', out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
