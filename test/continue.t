#!/usr/bin/env python3

###############################################################################
#
# Copyright 2016 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

from datetime import datetime, timedelta, timezone

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestContinue(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_continue_nothing(self):
        """Verify that continuing an empty db is an error"""
        code, out, err = self.t.runError("continue")
        self.assertIn("There is no previous tracking to continue.", err)

    def test_continue_open(self):
        """Verify that continuing an open interval is an error"""
        code, out, err = self.t("start tag1 tag2 1h ago")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t.runError("continue")
        self.assertIn("There is already active tracking.", err)

    def test_continue_closed(self):
        """Verify that continuing a closed interval works"""
        code, out, err = self.t("start tag1 tag2 1h ago")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t("stop")
        self.assertIn("Recorded tag1 tag2\n", out)

        code, out, err = self.t("continue")
        self.assertIn("Tracking tag1 tag2\n", out)

    def test_continue_with_multiple_ids(self):
        """Verify that 'continue' with multiple ids is an error"""
        code, out, err = self.t.runError("continue @1 @2")
        self.assertIn("You can only specify one ID to continue.\n", err)

    def test_continue_with_invalid_id(self):
        """Verify that 'continue' with invalid id is an error"""
        code, out, err = self.t("start FOO 1h ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop 30min ago")
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t.runError("continue @4")
        self.assertIn("ID '@4' does not correspond to any tracking.\n", err)

    def test_continue_with_id_without_active_tracking(self):
        """Verify that continuing a specified interval works"""
        code, out, err = self.t("start FOO 1h ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("start BAR 30min ago")
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop 15min ago")
        self.assertIn("Recorded BAR\n", out)

        code, out, err = self.t("continue @2")
        self.assertIn("Tracking FOO\n", out)

    def test_continue_with_id_with_active_tracking(self):
        """Verify that continuing a specified interval stops active tracking"""
        code, out, err = self.t("start FOO 1h ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("start BAR 30min ago")
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("continue @2")
        self.assertIn("Recorded BAR\n", out)
        self.assertIn("Tracking FOO\n", out)

    def test_continue_with_multiple_tags(self):
        """Verify that 'continue' with multiple tags works"""
        code, out, err = self.t("start FOO BAR 2h ago")
        self.assertIn("Tracking BAR FOO\n", out)

        code, out, err = self.t("start BAR 1h ago")
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("start FOO 30min ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("continue FOO BAR")
        self.assertIn("Recorded FOO\n", out)
        self.assertIn("Tracking BAR FOO\n", out)

    def test_continue_with_invalid_tag(self):
        """Verify that 'continue' with invalid tag is an error"""
        code, out, err = self.t("start FOO 1h ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop 30min ago")
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t.runError("continue BAR")
        self.assertIn("Tags 'BAR' do not correspond to any tracking.\n", err)

    def test_continue_with_tag_without_active_tracking(self):
        """Verify that continuing a specified interval works"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track {:%Y-%m-%dT%H}:00Z - {:%Y-%m-%dT%H}:00Z BAR".format(five_hours_before_utc, four_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H}:00Z - {:%Y-%m-%dT%H}:00Z FOO BAR".format(four_hours_before_utc, three_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H}:00Z - {:%Y-%m-%dT%H}:00Z FOO BAZ".format(three_hours_before_utc, two_hours_before_utc))

        self.t("continue BAR {:%Y-%m-%dT%H}:00Z".format(now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 4)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR", "FOO"],
                                  description="second interval")
        self.assertClosedInterval(j[2],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(two_hours_before_utc),
                                  expectedTags=["BAZ", "FOO"],
                                  description="third interval")
        self.assertOpenInterval(j[3],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(now_utc),
                                expectedTags=["BAR", "FOO"],
                                description="continued interval")

    def test_continue_with_tag_with_active_tracking(self):
        """Verify that continuing a specified interval stops active tracking"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track {:%Y-%m-%dT%H}:00Z - {:%Y-%m-%dT%H}:00Z BAR".format(five_hours_before_utc, four_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H}:00Z - {:%Y-%m-%dT%H}:00Z FOO BAR".format(four_hours_before_utc, three_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H}:00Z - {:%Y-%m-%dT%H}:00Z FOO BAZ".format(three_hours_before_utc, two_hours_before_utc))
        self.t("start {:%Y-%m-%dT%H}:00Z FOO".format(two_hours_before_utc))

        self.t("continue BAR {:%Y-%m-%dT%H}:00Z".format(now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 5)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR", "FOO"],
                                  description="second interval")
        self.assertClosedInterval(j[2],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(two_hours_before_utc),
                                  expectedTags=["BAZ", "FOO"],
                                  description="third interval")
        self.assertClosedInterval(j[3],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(two_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(now_utc),
                                  expectedTags=["FOO"],
                                  description="fourth interval")
        self.assertOpenInterval(j[4],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(now_utc),
                                expectedTags=["BAR", "FOO"],
                                description="continued interval")

    def test_continue_with_tag_and_date(self):
        """Verify that continuing an interval specified by tag with date continues at given date"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        code, out, err = self.t("start FOO {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc))
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t("start BAR {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(three_hours_before_utc))
        self.assertIn("Recorded BAR\n", out)

        self.t("continue FOO {:%Y-%m-%dT%H}:00:00Z".format(two_hours_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["FOO"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="second interval")
        self.assertOpenInterval(j[2],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(two_hours_before_utc),
                                expectedTags=["FOO"],
                                description="continued interval")

    def test_continue_with_tag_and_range(self):
        """Verify that continue an interval specified by tag with a range adds a copy with same tags at given range"""
        now_utc = datetime.now().utcnow()

        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        code, out, err = self.t("start FOO {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc))
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t("start BAR {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(three_hours_before_utc))
        self.assertIn("Recorded BAR\n", out)

        self.t("continue FOO {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(two_hours_before_utc, one_hour_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)

        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["FOO"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="second interval")
        self.assertClosedInterval(j[2],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(two_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(one_hour_before_utc),
                                  expectedTags=["FOO"],
                                  description="added interval")

    def test_continue_with_tag_and_id(self):
        """Verify that continuing an interval by specifying id and tags is an error"""
        code, out, err = self.t.runError("continue @2 FOO")
        self.assertIn("You cannot specify both id and tags to continue an interval.\n", err)

    def test_continue_with_id_and_date(self):
        """Verify that continuing a specified interval with date continues at given date"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        code, out, err = self.t("start FOO {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc))
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t("start BAR {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(three_hours_before_utc))
        self.assertIn("Recorded BAR\n", out)

        self.t("continue @2 {:%Y-%m-%dT%H}:00:00Z".format(two_hours_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["FOO"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="second interval")
        self.assertOpenInterval(j[2],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(two_hours_before_utc),
                                expectedTags=["FOO"],
                                description="continued interval")

    def test_continue_without_adjust_hint(self):
        """Verify that continuing without the :adjust hint fails to overwrite"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track FOO {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc, four_hours_before_utc))
        self.t("track BAR {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc, two_hours_before_utc))

        self.t.runError("continue @2 {:%Y-%m-%dT%H}:00:00Z".format(three_hours_before_utc))

    def test_continue_with_adjust_hint(self):
        """Verify that continuing with the :adjust hint works"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track FOO {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc, four_hours_before_utc))
        self.t("track BAR {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc, two_hours_before_utc))

        self.t("continue @2 {:%Y-%m-%dT%H}:00:00Z :adjust".format(three_hours_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["FOO"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="second interval")
        self.assertOpenInterval(j[2],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                expectedTags=["FOO"],
                                description="continued interval")

    def test_continue_with_id_is_idempotent(self):
        """Verify that continuing with id is idempotent"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track FOO {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc, four_hours_before_utc))
        self.t("track BAR {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc, three_hours_before_utc))
        self.t("start FOO {:%Y-%m-%dT%H}:00:00Z".format(three_hours_before_utc))
        self.t("continue @3 {:%Y-%m-%dT%H}:00:00Z".format(now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["FOO"],
                                  description="third interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="second interval")
        self.assertOpenInterval(j[2],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                expectedTags=["FOO"],
                                description="first interval")

    def test_continue_with_tag_is_idempotent(self):
        """Verify that continuing with id is idempotent"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track FOO {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc, four_hours_before_utc))
        self.t("track BAR {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc, three_hours_before_utc))
        self.t("start FOO {:%Y-%m-%dT%H}:00:00Z".format(three_hours_before_utc))
        self.t("continue FOO {:%Y-%m-%dT%H}:00:00Z".format(now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["FOO"],
                                  description="third interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="second interval")
        self.assertOpenInterval(j[2],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                expectedTags=["FOO"],
                                description="first interval")

    def test_continue_with_id_and_range(self):
        """Verify that continue with a range adds a copy with same tags"""
        now_utc = datetime.now().utcnow()

        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        code, out, err = self.t("start FOO {:%Y-%m-%dT%H}:00:00Z".format(five_hours_before_utc))
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t("start BAR {:%Y-%m-%dT%H}:00:00Z".format(four_hours_before_utc))
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00Z".format(three_hours_before_utc))
        self.assertIn("Recorded BAR\n", out)

        self.t("continue @2 {:%Y-%m-%dT%H}:00:00Z - {:%Y-%m-%dT%H}:00:00Z".format(two_hours_before_utc, one_hour_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 3)

        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedTags=["FOO"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(four_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(three_hours_before_utc),
                                  expectedTags=["BAR"],
                                  description="second interval")
        self.assertClosedInterval(j[2],
                                  expectedStart="{:%Y%m%dT%H}0000Z".format(two_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(one_hour_before_utc),
                                  expectedTags=["FOO"],
                                  description="added interval")

    def test_continue_with_future_time(self):
        """Verify that continue fails with time in the future"""
        now_utc = datetime.now(timezone.utc)

        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        code, out, err = self.t("start FOO {:%Y-%m-%dT%H}:00:00Z".format(two_hours_before_utc))
        self.assertIn("Tracking FOO\n", out)
        code, out, err = self.t("start BAR {:%Y-%m-%dT%H}:00:00Z".format(one_hour_before_utc))
        self.assertIn("Tracking BAR\n", out)
        code, out, err = self.t("stop")
        self.assertIn("Recorded BAR\n", out)

        input_time_utc = now_utc + timedelta(seconds=10)
        code, out, err = self.t("continue @2 from {:%Y-%m-%dT%H:%M:%S}Z".format(input_time_utc))
        self.assertIn("Scheduled for {:%Y-%m-%dT%H:%M:%S}".format(input_time_utc.astimezone()), out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
