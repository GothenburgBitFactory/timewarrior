#!/usr/bin/env python3

###############################################################################
#
# Copyright 2016 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

from datetime import datetime, timedelta

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestMove(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_move_closed_forwards(self):
        """Move a closed interval forwards in time"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")

        code, out, err = self.t("move @1 2016-01-01T02:00:00")

        self.assertIn('Moved @1 to 2016-01-01T02:00:00', out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_move_closed_backwards(self):
        """Move a closed interval backwards in time"""
        self.t("track 2016-01-01T04:00:00 - 2016-01-01T05:00:00 foo")

        code, out, err = self.t("move @1 2016-01-01T03:00:00")

        self.assertIn('Moved @1 to 2016-01-01T03:00:00', out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_move_open_backwards_to_specific_time(self):
        """Move an open interval backwards to specific time."""
        one_hour_before = datetime.now() - timedelta(hours=1)

        self.t("start 5mins ago foo")
        code, out, err = self.t("move @1 {:%Y-%m-%dT%H}:01:23".format(one_hour_before))
        self.assertRegex(out, "Moved @1 to {:%Y-%m-%dT%H}:01:23".format(one_hour_before))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["foo"])

    def test_move_open_backwards(self):
        """Move an open interval backwards in time"""
        now = datetime.now()
        five_hours_before = now - timedelta(hours=5)

        now_utc = now.utcnow()
        five_hours_before_utc = now_utc - timedelta(hours=5)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(two_hours_before_utc))

        code, out, err = self.t("move @1 {:%Y-%m-%dT%H:%M:%S}Z".format(five_hours_before_utc))
        self.assertRegex(out, 'Moved @1 to {:%Y-%m-%dT%H:%M:%S}'.format(five_hours_before))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                expectedTags=["foo"])

    def test_move_open_forwards(self):
        """Move an open interval forwards in time"""
        now = datetime.now()
        two_hours_before = now - timedelta(hours=2)

        now_utc = now.utcnow()
        five_hours_before_utc = now_utc - timedelta(hours=5)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        code, out, err = self.t("move @1 {:%Y-%m-%dT%H:%M:%S}Z".format(two_hours_before_utc))

        self.assertRegex(out, 'Moved @1 to {:%Y-%m-%dT%H:%M:%S}'.format(two_hours_before))

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(two_hours_before_utc),
                                expectedTags=["foo"])

    def test_move_interval_over_another_with_adjust(self):
        """Move an interval over another with :adjust"""
        self.t("track 20170301T110000Z - 20170301T140000Z foo")
        self.t("track 20170301T150000Z - 20170301T160000Z bar")

        code, out, err = self.t("move @1 20170301T133000Z :adjust")

        self.assertRegex(out, 'Moved @1 to 2017-03-01T\d\d:\d\d:\d\d\n')

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="20170301T110000Z",
                                  expectedEnd="20170301T133000Z",
                                  expectedTags=["foo"])
        self.assertClosedInterval(j[1],
                                  expectedStart="20170301T133000Z",
                                  expectedEnd="20170301T143000Z",
                                  expectedTags=["bar"])

    def test_move_synthetic_interval_into_exclusion(self):
        """Move a synthetic interval into exclusion"""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        self.t("move @2 {:%Y-%m-%dT%H:%M:%S}Z".format(five_hours_before_utc + timedelta(minutes=20)))

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart=five_hours_before_utc + timedelta(minutes=20),
                                  expectedEnd=four_hours_before_utc + timedelta(minutes=20),
                                  expectedTags=[],
                                  description="moved interval")
        self.assertOpenInterval(j[1],
                                expectedStart=three_hours_before_utc,
                                expectedTags=[],
                                description="unmodified interval")

    def test_move_synthetic_interval_away_from_exclusion(self):
        """Move a synthetic interval away from exclusion"""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        self.t("move @2 {:%Y-%m-%dT%H:%M:%S}Z".format(five_hours_before_utc - timedelta(minutes=20)))

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart=five_hours_before_utc - timedelta(minutes=20),
                                  expectedEnd=four_hours_before_utc - timedelta(minutes=20),
                                  expectedTags=[],
                                  description="moved interval")
        self.assertOpenInterval(j[1],
                                expectedStart=three_hours_before_utc,
                                expectedTags=[],
                                description="unmodified interval")

    def test_move_synthetic_interval_into_exclusion(self):
        """timew move should work with both synthetic and non-synethic intervals in database"""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        day_before = now_utc - timedelta(days=1)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        # Place a non-synthetic interval in the history
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z bar".format(day_before, day_before + timedelta(minutes=30)))

        # Now create an open interval that crosses the exlusions added
        # previously, which will result in synthetic intervals
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart=day_before,
                                  expectedEnd=day_before + timedelta(minutes=30),
                                  expectedTags=[],
                                  description="unmodified interval")
        self.assertClosedInterval(j[1],
                                  expectedStart=five_hours_before_utc,
                                  expectedEnd=four_hours_before_utc,
                                  expectedTags=[],
                                  description="unmodified interval")
        self.assertOpenInterval(j[2],
                                expectedStart=three_hours_before_utc,
                                expectedTags=[],
                                description="unmodified interval")

        # First move the non-synthetic one
        self.t("move @3 {:%Y-%m-%dT%H:%M:%S}Z".format(day_before + timedelta(minutes=30)))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart=day_before + timedelta(minutes=30),
                                  expectedEnd=day_before + timedelta(hours=1),
                                  expectedTags=[],
                                  description="moved interval")
        self.assertClosedInterval(j[1],
                                  expectedStart=five_hours_before_utc,
                                  expectedEnd=four_hours_before_utc,
                                  expectedTags=[],
                                  description="unmodified interval")
        self.assertOpenInterval(j[2],
                                expectedStart=three_hours_before_utc,
                                expectedTags=[],
                                description="unmodified interval")

        # Then move the synthetic one
        self.t("move @2 {:%Y-%m-%dT%H:%M:%S}Z".format(five_hours_before_utc + timedelta(minutes=20)))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart=day_before + timedelta(minutes=30),
                                  expectedEnd=day_before + timedelta(hours=1),
                                  expectedTags=[],
                                  description="non-synthetic interval")
        self.assertClosedInterval(j[1],
                                  expectedStart=five_hours_before_utc + timedelta(minutes=20),
                                  expectedEnd=four_hours_before_utc + timedelta(minutes=20),
                                  expectedTags=[],
                                  description="moved interval")
        self.assertOpenInterval(j[2],
                                expectedStart=three_hours_before_utc,
                                expectedTags=[],
                                description="unmodified interval")


    def test_move_interval_to_enclose_a_month_border(self):
        """Move an interval to enclose a month border"""
        self.t("track 20180831T180000 - 20180831T230000 foo")
        self.t("move @1 20180831T220000")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0])

    # TODO Add :adjust tests.


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
