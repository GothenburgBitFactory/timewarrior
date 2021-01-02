#!/usr/bin/env python3

###############################################################################
#
# Copyright 2016 - 2021, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
# https://www.opensource.org/licenses/mit-license.php
#
###############################################################################

import os
import sys
import unittest

from datetime import datetime, timedelta, time

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
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_single_interval_enclosing_exclusion(self):
        """Add one interval that encloses an exclusion, and is therefore flattened"""
        self.t.configure_exclusions([(time(18, 0, 0), time(9, 0, 0)),
                                    (time(12, 0, 0), time(13, 0, 0))])

        self.t("track 20160101T100000 - 20160101T150000 foo")

        j = self.t.export()
        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertClosedInterval(j[1], expectedTags=["foo"])

    def test_single_interval_extending_into_exclusions(self):
        """Add one interval that extends at either end into exclusions"""
        self.t.configure_exclusions([(time(18, 0, 0), time(9, 0, 0))])

        self.t("track 20160527T085959 - 20160527T180101 foo")

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0])
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

    def test_track_with_new_tag(self):
        """Call 'track' with new tag"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} foo".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} bar".format(one_hour_before_utc, now_utc))

        self.assertIn("Note: 'bar' is a new tag", out)
        self.assertIn("Recorded bar", out)

    def test_track_with_previous_tag(self):
        """Call 'track' with previous tag"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} bar".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} bar".format(one_hour_before_utc, now_utc))

        self.assertNotIn("Note: 'bar' is a new tag", out)
        self.assertIn("Recorded bar", out)

    def test_track_with_future_time(self):
        """Test track with future interval is not an error"""
        now_utc = datetime.now().utcnow()

        two_hours_after_utc = now_utc + timedelta(hours=2)
        one_hour_after_utc = now_utc + timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} bar".format(one_hour_after_utc, two_hours_after_utc))

    def test_track_interval_which_encloses_month_border(self):
        """Track an interval which encloses a month border"""
        self.t("track 20180831T220000 - 20180901T030000 foo")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0])

    def test_track_with_adjust_should_overwrite_enclosed_interval_with_same_start(self):
        """Command track with adjust should overwrite enclosed interval with same start"""
        now_utc = datetime.now().utcnow()

        four_hours_before = now_utc - timedelta(hours=4)
        three_hours_before = now_utc - timedelta(hours=3)
        two_hours_before = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z FOO".format(four_hours_before, three_hours_before))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z BAR :adjust".format(four_hours_before, two_hours_before))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before,
                                  expectedEnd=two_hours_before,
                                  expectedTags=["BAR"])

    def test_track_with_adjust_should_overwrite_enclosed_interval_with_same_end(self):
        """Command track with adjust should overwrite enclosed interval with same end"""
        now_utc = datetime.now().utcnow()

        five_hours_before = now_utc - timedelta(hours=5)
        four_hours_before = now_utc - timedelta(hours=4)
        three_hours_before = now_utc - timedelta(hours=3)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z FOO".format(four_hours_before, three_hours_before))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z BAR :adjust".format(five_hours_before, three_hours_before))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart=five_hours_before,
                                  expectedEnd=three_hours_before,
                                  expectedTags=["BAR"])

    def test_track_with_adjust_should_overwrite_identical_interval(self):
        """Command track with adjust should overwrite identical interval"""
        now_utc = datetime.now().utcnow()

        four_hours_before = now_utc - timedelta(hours=4)
        three_hours_before = now_utc - timedelta(hours=3)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z FOO".format(four_hours_before, three_hours_before))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z BAR :adjust".format(four_hours_before, three_hours_before))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before,
                                  expectedEnd=three_hours_before,
                                  expectedTags=["BAR"])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
