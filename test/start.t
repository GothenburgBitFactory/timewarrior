#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2006 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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


class TestStart(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_start(self):
        """Test trivial start"""
        self.t("start")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0])

    def test_timed_start_past(self):
        """Test timed start past"""
        self.t("start 00:00am")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0])

    def test_timed_start_future(self):
        """Test timed start future"""
        code, out, err = self.t.runError("start 11:59pm")

        self.assertIn("Time tracking cannot be set in the future.", err)

    def test_start_with_open_interval(self):
        """Test start with already open interval, which should be auto-stopped"""
        self.t("start 2016-01-01T00:00:00 foo")

        code, out, err = self.t("start bar")

        self.assertIn("Recorded foo", out)
        self.assertIn("Tracking bar", out)

    def test_start_with_open_interval_timed(self):
        """Test timed start with already open interval, which should be auto-stopped at given datetime"""
        self.t("start 2016-01-01T00:00:00 foo")

        code, out, err = self.t("start 2016-01-01T01:00:00 bar")

        self.assertIn("Recorded foo", out)
        self.assertIn("Tracking bar", out)
        self.assertIn("Ended              01:00:00", out)

    def test_start_with_more_tags_than_current_tracking(self):
        """Test 'start' with more tags than current tracking should start new tracking"""
        self.t("start 1h ago foo")

        code, out, err = self.t("start foo bar")

        self.assertIn("Recorded foo", out)
        self.assertIn("Tracking bar foo", out)

    def test_start_with_less_tags_than_current_tracking(self):
        """Test 'start' with less tags than current tracking should start new tracking"""
        self.t("start 1h ago foo bar")

        code, out, err = self.t("start foo")

        self.assertIn("Recorded bar foo", out)
        self.assertIn("Tracking foo", out)

    def test_start_with_same_tags_as_current_tracking(self):
        """Test 'start' with same tags as current tracking should not start new tracking"""
        self.t("start 1h ago bar foo")

        code, out, err = self.t("start foo bar")

        self.assertNotIn("Recorded bar foo", out)

    def test_single_interval_enclosing_exclusion(self):
        """Add one interval that encloseѕ an exclusion, and is therefore flattened"""
        self.t.configure_exclusions([(time(18, 5, 11), time(9, 11, 50)),
                                     (time(12, 22, 44), time(13, 32, 23))])

        self.t("start 20160101T102255 foo")
        self.t("start 20160101T154422 bar")  # start bar, so foo gets closed and flattened
        self.t("cancel")  # cancel tracking of bar

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('2255Z', j[0]['start'])
        self.assertIn('2244Z', j[0]['end'])

        self.assertClosedInterval(j[1], expectedTags=["foo"])
        self.assertIn('3223Z', j[1]['start'])
        self.assertIn('4422Z', j[1]['end'])

    def test_single_interval_starting_within_an_exclusion_and_enclosing_an_exclusion(self):
        """Add one interval that starts within an exclusion and encloses an exclusion"""
        self.t.configure_exclusions([(time(18, 5, 11), time(9, 11, 50)),
                                     (time(12, 22, 44), time(13, 32, 23))])

        self.t("start 20160101T082255 foo")
        self.t("start 20160101T144422 bar")  # start bar, so foo gets closed and flattened
        self.t("cancel")  # cancel tracking of bar

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('2255Z', j[0]['start'])
        self.assertIn('2244Z', j[0]['end'])

        self.assertClosedInterval(j[1], expectedTags=["foo"])
        self.assertIn('3223Z', j[1]['start'])
        self.assertIn('4422Z', j[1]['end'])

    def test_single_interval_ending_within_an_exclusion_and_enclosing_an_exclusion(self):
        """Add one interval that ends within an exclusion and encloses an exclusion"""
        self.t.configure_exclusions([(time(18, 5, 11), time(9, 11, 50)),
                                     (time(12, 22, 44), time(13, 32, 23))])

        self.t("start 20160101T102255 foo")
        self.t("start 20160101T194422 bar")  # start bar, so foo gets closed and flattened
        self.t("cancel")  # cancel tracking of bar

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('2255Z', j[0]['start'])
        self.assertIn('2244Z', j[0]['end'])

        self.assertClosedInterval(j[1], expectedTags=["foo"])
        self.assertIn('3223Z', j[1]['start'])
        self.assertIn('4422Z', j[1]['end'])

    def test_single_interval_and_enclosing_an_exclusion_with_day_change(self):
        """Add one interval that encloses an exclusion with day change"""
        self.t.configure_exclusions([(time(18, 5, 11), time(9, 11, 50)),
                                     (time(12, 22, 44), time(13, 32, 23))])

        self.t("start 20160101T172255 foo")
        self.t("start 20160102T104422 bar")  # start bar, so foo gets closed and flattened
        self.t("cancel")  # cancel tracking of bar

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('2255Z', j[0]['start'])
        self.assertIn('0511Z', j[0]['end'])

        self.assertClosedInterval(j[1], expectedTags=["foo"])
        self.assertIn('1150Z', j[1]['start'])
        self.assertIn('4422Z', j[1]['end'])

    def test_start_with_new_tag(self):
        """Call 'start' with new tag"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} foo".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("start {:%Y-%m-%dT%H:%M:%S} bar".format(now_utc))

        self.assertIn("Note: 'bar' is a new tag", out)
        self.assertIn("Tracking bar", out)

    def test_start_with_previous_tag(self):
        """Call 'start' with previous tag"""
        now_utc = datetime.now().utcnow()

        three_hours_before_utc = now_utc - timedelta(hours=3)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} bar".format(three_hours_before_utc, two_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} foo".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("start {:%Y-%m-%dT%H:%M:%S} bar".format(now_utc))

        self.assertNotIn("Note: 'bar' is a new tag", out)
        self.assertIn("Tracking bar", out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
