#!/usr/bin/env python
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

from datetime import datetime, timedelta

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


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
