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


class TestStop(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_timed_stop(self):
        """Test timed stop"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart=one_hour_before_utc,
                                  expectedEnd=now_utc)

    def test_stop_with_end_before_start_is_an_error(self):
        """Verify stop date before start date is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))

        code, out, err = self.t.runError("stop {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        self.assertIn("The end of a date range must be after the start.", err)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=[])

    def test_stop_all(self):
        """Start three tags, stop"""
        self.t("start 5mins ago one two three")

        code, out, err = self.t("stop")

        self.assertIn("Recorded one three two", out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["one", "two", "three"])

    def test_stop_three(self):
        """Start three tags, stop three"""
        self.t("start 5mins ago one two three")

        code, out, err = self.t("stop one two three")

        self.assertIn("Recorded one three two", out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["one", "two", "three"])

    def test_stop_two(self):
        """Start three tags, stop two"""
        self.t("start 5mins ago one two three")

        code, out, err = self.t("stop one three")

        self.assertIn("Tracking two", out)

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["one", "two", "three"])
        self.assertOpenInterval(j[1], expectedTags=["two"])

    def test_stop_fourth(self):
        """Start three tags, stop fourth"""
        self.t("start 5mins ago one two three")

        code, out, err = self.t.runError("stop four")

        self.assertIn("The current interval does not have the 'four' tag.", err)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["one", "two", "three"])

    def test_stop_non_existing_tag(self):
        """Start empty, stop with tag"""
        self.t("start 5mins ago ")

        code, out, err = self.t.runError("stop bogus")

        self.assertIn("The current interval does not have the 'bogus' tag.", err)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=[])

    def test_stop_with_all_hint_is_an_error(self):
        """Verify stop with :all hint is an error"""
        self.t("start 5mins ago FOO ")

        code, out, err = self.t.runError("stop :all")

        self.assertIn("No datetime specified.", err)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["FOO"])

    def test_single_interval_enclosing_exclusion(self):
        """Add one interval that encloses an exclusion, and is therefore flattened"""
        self.t.configure_exclusions([(time(18, 5, 11), time(9, 11, 50)),
                                     (time(12, 22, 44), time(13, 32, 23))])

        self.t("start 20160101T100000 foo")
        self.t("stop 20160101T150000")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertClosedInterval(j[1], expectedTags=["foo"])

    def test_single_interval_starting_within_an_exclusion_and_enclosing_an_exclusion(self):
        """Add one interval that starts within an exclusion and encloses an exclusion"""
        self.t.configure_exclusions([(time(18, 5, 11), time(9, 11, 50)),
                                     (time(12, 22, 44), time(13, 32, 23))])

        self.t("start 20160101T082255 foo")
        self.t("stop 20160101T144422")  # stop, so foo gets closed and flattened

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
        self.t("stop 20160101T194422")  # stop, so foo gets closed and flattened

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('2255Z', j[0]['start'])
        self.assertIn('2244Z', j[0]['end'])

        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('3223Z', j[1]['start'])
        self.assertIn('4422Z', j[1]['end'])

    def test_single_interval_and_enclosing_an_exclusion_with_day_change(self):
        """Add one interval that encloses an exclusion with day change"""
        self.t.configure_exclusions([(time(18, 5, 11), time(9, 11, 50)),
                                     (time(12, 22, 44), time(13, 32, 23))])

        self.t("start 20160101T172255 foo")
        self.t("stop 20160102T104422")  # stop, so foo gets closed and flattened

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('2255Z', j[0]['start'])
        self.assertIn('0511Z', j[0]['end'])

        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertIn('1150Z', j[1]['start'])
        self.assertIn('4422Z', j[1]['end'])

    def test_stop_tracking_of_interval_which_encloses_month_border(self):
        """Stop tracking of an interval which encloses a month border"""
        self.t("start 20180831T220000 foo")
        self.t("stop 20180901T030000")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0])

    def test_stop_with_id_should_suggest_modify_command(self):
        """Stop does not work with with ids, suggest modify command instead"""
        self.t("start 2h ago")
        self.t("start 1h ago")
        
        code, out, err = self.t.runError("stop @1")
        # If trying to stop the latest interval, check that modify was suggested.
        self.assertIn("modify", err)

        code, out, err = self.t.runError("stop @2")
        # If trying to stop an older interval, check that modify was suggested.
        self.assertIn("modify", err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
