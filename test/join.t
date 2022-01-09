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
import unittest
from datetime import datetime, timedelta

import sys

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestJoin(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_join_closed_intervals(self):
        """Join two closed intervals"""
        now = datetime.now()
        now_utc = now.utcnow()

        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc, four_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z bar".format(two_hours_before_utc, one_hour_before_utc))

        code, out, err = self.t("join @1 @2")

        self.assertIn('Joined @1 and @2', out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(one_hour_before_utc),
                                  expectedTags=["foo", "bar"])

    def test_join_closed_and_open_interval(self):
        """Join closed and open interval"""
        now = datetime.now()
        now_utc = now.utcnow()

        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc, four_hours_before_utc))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z bar".format(two_hours_before_utc, one_hour_before_utc))

        code, out, err = self.t("join @1 @2")

        self.assertIn('Joined @1 and @2', out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                expectedTags=["foo", "bar"])

    def test_referencing_a_non_existent_interval_is_an_error(self):
        """Calling join with a non-existent interval reference is an error"""
        code, out, err = self.t.runError("join @1 @2")
        self.assertIn("ID '@1' does not correspond to any tracking.", err)

        self.t("start 1h ago bar")

        code, out, err = self.t.runError("join @1 @2")
        self.assertIn("ID '@2' does not correspond to any tracking.", err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
