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

from datetime import datetime, timedelta

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestTrack(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_track(self):
        """Test trivial track"""
        code, out, err = self.t("track")
        self.assertIn("Started", out)

    def test_single_unobstructed_interval(self):
        """Add one interval and export it as-is"""
        self.t("track 20160101T080000 - 20160101T090000 foo")

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_single_interval_enclosing_exclusion(self):
        """Add one interval that encloseѕ an exclusion, and is therefore flattened"""
        self.t.config("exclusions.monday",    "<9:00 12:00-13:00 >18:00")
        self.t.config("exclusions.tuesday",   "<9:00 12:00-13:00 >18:00")
        self.t.config("exclusions.wednesday", "<9:00 12:00-13:00 >18:00")
        self.t.config("exclusions.thursday",  "<9:00 12:00-13:00 >18:00")
        self.t.config("exclusions.friday",    "<9:00 12:00-13:00 >18:00")
        self.t.config("exclusions.saturday",  "<9:00 12:00-13:00 >18:00")
        self.t.config("exclusions.sunday",    "<9:00 12:00-13:00 >18:00")

        self.t("track 20160101T100000 - 20160101T150000 foo")
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

    def test_single_interval_extending_into_exclusions(self):
        """Add one interval that extends at either end into exclusions"""
        self.t.config("exclusions.monday",    "<9:00 >18:00")
        self.t.config("exclusions.tuesday",   "<9:00 >18:00")
        self.t.config("exclusions.wednesday", "<9:00 >18:00")
        self.t.config("exclusions.thursday",  "<9:00 >18:00")
        self.t.config("exclusions.friday",    "<9:00 >18:00")
        self.t.config("exclusions.saturday",  "<9:00 >18:00")
        self.t.config("exclusions.sunday",    "<9:00 >18:00")

        self.t("track 20160527T085959 - 20160527T180101 foo")
        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertIn('5959', j[0]['start'])
        self.assertIn('0101', j[0]['end'])

    def test_overlap_prevention(self):
        """Test adding an overlapping interval fails"""
        self.t("track 20160709T1400 - 20160709T1500 foo")
        code, out, err = self.t.runError("track 20160709T1430 - 20160709T1530 foo")
        self.assertIn('You cannot overlap intervals. Correct the start/end time, or specify the :adjust hint.', err)

    def test_track_at_time(self):
        """Test adding time in the past, using only times"""
        one_hour_before = datetime.now() - timedelta(hours=1)

        self.t('track {0:%Y-%m-%dT%H}:01:00 - {0:%Y-%m-%dT%H}:02:00 "Test track interval"'.format(one_hour_before))
        j = self.t.export()
        self.assertTrue(len(j) > 0)


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
