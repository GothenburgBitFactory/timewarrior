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
from datetime import datetime, timedelta
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

class TestShorten(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_shorten_closed_interval(self):
        """Shorten a closed interval"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")
        code, out, err = self.t("shorten @1 10mins")
        self.assertIn('Shortened @1 by 0:10:00', out)

    def test_shorten_open_interval(self):
        """Shorten an open interval"""
        self.t("start 30mins ago foo")
        code, out, err = self.t.runError("shorten @1 10mins")
        self.assertIn('Cannot shorten open interval @1', err)

    def test_shorten_interval_moved_into_exclusion(self):
        """Shorten an interval moved to span an exclusion."""
        self.t.config("exclusions.friday", "<7:30 12:00-13:00 >16:30")
        self.t.config("exclusions.thursday", "<7:30 12:00-13:00 >16:30")
        self.t.config("exclusions.wednesday", "<7:30 12:00-13:00 >16:30")
        self.t.config("exclusions.tuesday", "<7:30 12:00-13:00 >16:30")
        self.t.config("exclusions.monday", "<7:30 12:00-13:00 >16:30")
        self.t.config("exclusions.sunday", "<7:30 12:00-13:00 >16:30")
        self.t.config("exclusions.saturday", "<7:30 12:00-13:00 >16:30")

        self.t('track 20170308T140000 - 20170308T161500')

        # self.t("shorten @1 5min") # This would work.
        self.t("move @1 20170308T113000")
        self.t("shorten @1 5min")  # Does not work.

    def test_shorten_synthetic_interval(self):
        """Shorten a synthetic interval."""
        now = datetime.now()
        now_utc = now.utcnow()

        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        exclusion = "{:%H}:00-{:%H}:00".format(four_hours_before, three_hours_before)

        self.t.config("exclusions.friday", exclusion)
        self.t.config("exclusions.thursday", exclusion)
        self.t.config("exclusions.wednesday", exclusion)
        self.t.config("exclusions.tuesday", exclusion)
        self.t.config("exclusions.monday", exclusion)
        self.t.config("exclusions.sunday", exclusion)
        self.t.config("exclusions.saturday", exclusion)

        self.t("start {}T{:%H}:45:00".format(now.date(), five_hours_before))

        self.t("shorten @2 5min")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}4500Z'.format(now_utc - timedelta(hours=5)), 'start time of lengthened interval does not match')
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}5500Z'.format(now_utc - timedelta(hours=5)), 'end time of lengthened interval does not match')
        self.assertFalse('tags' in j[0])
        self.assertTrue('start' in j[1])
        self.assertEqual(j[1]['start'], '{:%Y%m%dT%H}0000Z'.format(now_utc - timedelta(hours=3)), 'start time of unmodified interval does not match')
        self.assertFalse('end' in j[1])
        self.assertFalse('tags' in j[1])


# TODO Add :adjust tests.
class TestBug6(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_over_shorten_closed_interval(self):
        """TI-6: Exception after shortening task.

           When an interval is shortened by an amount that exceeds it's length,
           an assert is triggered, and should be an error instead.
        """
        self.t("track 2016-06-08T07:30:00 - 2016-06-08T07:35:00 foo")
        code, out, err = self.t.runError("shorten @1 10mins")
        self.assertIn('Cannot shorten interval @1 by 0:10:00 because it is only 0:05:00 in length.', err)

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
