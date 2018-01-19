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

import os
import sys
import unittest

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


class TestStop(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_timed_stop(self):
        """Test timed stop"""
        self.t("start 20160516T090100")
        self.t("stop 20160516T100200")
        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertIn('0100Z', j[0]['start'])
        self.assertIn('0200Z', j[0]['end'])

    def test_invalid_stop(self):
        """Verify stop date after start date is an error"""
        self.t("start 20160516T100200")
        code, out, err = self.t.runError("stop 20160516T090100")
        self.assertIn("The end of a date range must be after the start.", err)

    def test_stop_all(self):
        """Start three tags, stop"""
        self.t("start 5mins ago one two three")
        code, out, err = self.t("stop")
        self.assertIn("Recorded one three two", out)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertItemsEqual(['one', 'two', 'three'], j[0]['tags'])

    def test_stop_three(self):
        """Start three tags, stop three"""
        self.t("start 5mins ago one two three")
        code, out, err = self.t("stop one two three")
        self.assertIn("Recorded one three two", out)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertItemsEqual(['one', 'two', 'three'], j[0]['tags'])

    def test_stop_two(self):
        """Start three tags, stop two"""
        self.t("start 5mins ago one two three")
        code, out, err = self.t("stop one     three")
        self.assertIn("Tracking two", out)

        j = self.t.export()
        self.assertEqual(len(j), 2)
        self.assertItemsEqual(['one', 'two', 'three'], j[0]['tags'])
        self.assertItemsEqual(['two'], j[1]['tags'])

    def test_stop_fourth(self):
        """Start three tags, stop fourth"""
        self.t("start 5mins ago one two three")
        code, out, err = self.t.runError("stop four")
        self.assertIn("The current interval does not have the 'four' tag.", err)

    def test_single_interval_enclosing_exclusion(self):
        """Add one interval that encloseѕ an exclusion, and is therefore flattened"""
        self.t.config("exclusions.monday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.tuesday",   "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.wednesday", "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.thursday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.friday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.saturday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.sunday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")

        self.t("start 20160101T100000 foo")
        self.t("stop 20160101T150000")

        j = self.t.export()
        self.assertEqual(len(j), 2)

        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertTrue('end' in j[1])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'][0], 'foo')

    def test_single_interval_starting_within_an_exclusion_and_enclosing_an_exclusion(self):
        """Add one interval that starts within an exclusion and encloses an exclusion"""
        self.t.config("exclusions.monday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.tuesday",   "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.wednesday", "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.thursday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.friday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.saturday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.sunday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")

        self.t("start 20160101T082255 foo")
        self.t("stop 20160101T144422")  # stop, so foo gets closed and flattened

        j = self.t.export()
        self.assertEqual(len(j), 2)

        self.assertTrue('start' in j[0])
        self.assertIn('2255Z', j[0]['start'])
        self.assertTrue('end' in j[0])
        self.assertIn('2244Z', j[0]['end'])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertIn('3223Z', j[1]['start'])
        self.assertTrue('end' in j[1])
        self.assertIn('4422Z', j[1]['end'])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'][0], 'foo')

    def test_single_interval_ending_within_an_exclusion_and_enclosing_an_exclusion(self):
        """Add one interval that ends within an exclusion and encloses an exclusion"""
        self.t.config("exclusions.monday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.tuesday",   "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.wednesday", "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.thursday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.friday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.saturday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.sunday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")

        self.t("start 20160101T102255 foo")
        self.t("stop 20160101T194422")  # stop, so foo gets closed and flattened

        j = self.t.export()
        self.assertEqual(len(j), 2)

        self.assertTrue('start' in j[0])
        self.assertIn('2255Z', j[0]['start'])
        self.assertTrue('end' in j[0])
        self.assertIn('2244Z', j[0]['end'])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertIn('3223Z', j[1]['start'])
        self.assertTrue('end' in j[1])
        self.assertIn('4422Z', j[1]['end'])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'][0], 'foo')

    def test_single_interval_and_enclosing_an_exclusion_with_day_change(self):
        """Add one interval that encloses an exclusion with day change"""
        self.t.config("exclusions.monday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.tuesday",   "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.wednesday", "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.thursday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.friday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.saturday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.sunday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")

        self.t("start 20160101T172255 foo")
        self.t("stop 20160102T104422")  # stop, so foo gets closed and flattened

        j = self.t.export()
        self.assertEqual(len(j), 2)

        self.assertTrue('start' in j[0])
        self.assertIn('2255Z', j[0]['start'])
        self.assertTrue('end' in j[0])
        self.assertIn('0511Z', j[0]['end'])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertIn('1150Z', j[1]['start'])
        self.assertTrue('end' in j[1])
        self.assertIn('4422Z', j[1]['end'])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'][0], 'foo')


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
