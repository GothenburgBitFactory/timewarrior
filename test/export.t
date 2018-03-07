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

import datetime

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestExport(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_export(self):
        """Test trivial export"""
        code, out, err = self.t("export")
        self.assertIn("[\n]\n", out)

    def test_single_unobstructed_interval(self):
        """Single unobstructed interval"""
        self.t("track 20160531T0800 - 20160531T0900 foo")

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_changing_exclusion_does_not_change_flattened_intervals(self):
        """Changing exclusions does not change flattened intervals"""
        self.t.config("exclusions.monday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.tuesday",   "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.wednesday", "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.thursday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.friday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.saturday",  "<9:11:50 12:22:44-13:32:23 >18:05:11")
        self.t.config("exclusions.sunday",    "<9:11:50 12:22:44-13:32:23 >18:05:11")

        self.t("track 20160101T102255 - 20160101T154422 foo")

        self.t.config("exclusions.monday",    "<9:11:50 12:44:22-13:23:32 >18:05:11")
        self.t.config("exclusions.tuesday",   "<9:11:50 12:44:22-13:23:32 >18:05:11")
        self.t.config("exclusions.wednesday", "<9:11:50 12:44:22-13:23:32 >18:05:11")
        self.t.config("exclusions.thursday",  "<9:11:50 12:44:22-13:23:32 >18:05:11")
        self.t.config("exclusions.friday",    "<9:11:50 12:44:22-13:23:32 >18:05:11")
        self.t.config("exclusions.saturday",  "<9:11:50 12:44:22-13:23:32 >18:05:11")
        self.t.config("exclusions.sunday",    "<9:11:50 12:44:22-13:23:32 >18:05:11")

        self.t("track 20160102T102255 - 20160102T154422 bar")

        j = self.t.export()
        self.assertEqual(len(j), 4)

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

        self.assertTrue('start' in j[2])
        self.assertIn('2255Z', j[2]['start'])
        self.assertTrue('end' in j[2])
        self.assertIn('4422Z', j[2]['end'])
        self.assertTrue('tags' in j[2])
        self.assertEqual(j[2]['tags'][0], 'bar')

        self.assertTrue('start' in j[3])
        self.assertIn('2332Z', j[3]['start'])
        self.assertTrue('end' in j[3])
        self.assertIn('4422Z', j[3]['end'])
        self.assertTrue('tags' in j[3])
        self.assertEqual(j[3]['tags'][0], 'bar')

    def test_changing_exclusion_does_change_open_interval(self):
        """Changing exclusions does change open interval"""
        now = datetime.datetime.now()

        three_hours_before = now - datetime.timedelta(hours=3)
        four_hours_before = now - datetime.timedelta(hours=4)
        five_hours_before = now - datetime.timedelta(hours=5)

        if four_hours_before.day < three_hours_before.day:
            exclusion = "<{:%H}:34:43 >{:%H}:12:21".format(three_hours_before, four_hours_before)
        else:
            exclusion = "{:%H}:12:21-{:%H}:34:43".format(four_hours_before, three_hours_before)

        # exclusion = "{:%H}:12:21-{:%H}:34:43".format(four_hours_before, three_hours_before)

        self.t.config("exclusions.friday", exclusion)
        self.t.config("exclusions.thursday", exclusion)
        self.t.config("exclusions.wednesday", exclusion)
        self.t.config("exclusions.tuesday", exclusion)
        self.t.config("exclusions.monday", exclusion)
        self.t.config("exclusions.sunday", exclusion)
        self.t.config("exclusions.saturday", exclusion)

        self.t("start {:%Y-%m-%dT%H}:00:00 foo".format(five_hours_before))

        j = self.t.export()

        self.assertEqual(len(j), 2)

        self.assertTrue('start' in j[0])
        self.assertIn('0000Z', j[0]['start'])
        self.assertTrue('end' in j[0])
        self.assertIn('1221Z', j[0]['end'])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertIn('3443Z', j[1]['start'])
        self.assertFalse('end' in j[1])

        if four_hours_before.day < three_hours_before.day:
            exclusion = "<{:%H}:12:21 >{:%H}:34:43".format(three_hours_before, four_hours_before)
        else:
            exclusion = "{:%H}:34:43-{:%H}:12:21".format(four_hours_before, three_hours_before)

        self.t.config("exclusions.friday", exclusion)
        self.t.config("exclusions.thursday", exclusion)
        self.t.config("exclusions.wednesday", exclusion)
        self.t.config("exclusions.tuesday", exclusion)
        self.t.config("exclusions.monday", exclusion)
        self.t.config("exclusions.sunday", exclusion)
        self.t.config("exclusions.saturday", exclusion)

        j = self.t.export()

        self.assertEqual(len(j), 2)

        self.assertTrue('start' in j[0])
        self.assertIn('0000Z', j[0]['start'])
        self.assertTrue('end' in j[0])
        self.assertIn('3443Z', j[0]['end'])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertIn('1221Z', j[1]['start'])
        self.assertFalse('end' in j[1])


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
