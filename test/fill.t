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
from datetime import datetime, timezone, timedelta
from dateutil import tz

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase

class TestFillCommand(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_fill_interval(self):
        """Expand interval to fill gaps"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)

        one_hour_before_utc = now_utc - timedelta(hours=1)
        one_hour_thirty_before_utc = now_utc - timedelta(hours=1, minutes=30)
        two_hour_thirty_before_utc = now_utc - timedelta(hours=2, minutes=30)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        three_hour_thirty_before_utc = now_utc - timedelta(hours=3, minutes=30)
        four_hours_before_utc = now_utc - timedelta(hours=4)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_three".format(four_hours_before_utc,
                                                                                       three_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_two".format(three_hours_before_utc,
                                                                                     two_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_one".format(one_hour_thirty_before_utc,
                                                                                     one_hour_before_utc))

        code, out, err = self.t("fill @2")

        self.assertIn('Backfilled', out)
        self.assertIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 3)

        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before_utc,
                                  expectedEnd=three_hour_thirty_before_utc,
                                  expectedTags=['task_three'])

        self.assertClosedInterval(j[1],
                                  expectedStart=three_hour_thirty_before_utc,
                                  expectedEnd=one_hour_thirty_before_utc,
                                  expectedTags=['task_two'])

        self.assertClosedInterval(j[2],
                                  expectedStart=one_hour_thirty_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=['task_one'])

    def test_fill_first_interval(self):
        """Expand first interval to fill gaps"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)

        one_hour_before_utc = now_utc - timedelta(hours=1)
        one_hour_thirty_before_utc = now_utc - timedelta(hours=1, minutes=30)
        two_hour_thirty_before_utc = now_utc - timedelta(hours=2, minutes=30)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_two".format(three_hours_before_utc,
                                                                                     two_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_one".format(one_hour_thirty_before_utc,
                                                                                     one_hour_before_utc))

        code, out, err = self.t("fill @2")

        self.assertNotIn('Backfilled', out)
        self.assertIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 2)

        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=one_hour_thirty_before_utc,
                                  expectedTags=['task_two'])

        self.assertClosedInterval(j[1],
                                  expectedStart=one_hour_thirty_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=['task_one'])

    def test_fill_latest_interval(self):
        """Expand latest interval to fill gaps"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)

        one_hour_before_utc = now_utc - timedelta(hours=1)
        one_hour_thirty_before_utc = now_utc - timedelta(hours=1, minutes=30)
        two_hour_thirty_before_utc = now_utc - timedelta(hours=2, minutes=30)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_two".format(three_hours_before_utc,
                                                                                     two_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_one".format(one_hour_thirty_before_utc,
                                                                                     one_hour_before_utc))

        code, out, err = self.t("fill @1")

        self.assertIn('Backfilled', out)
        self.assertNotIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 2)

        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=two_hour_thirty_before_utc,
                                  expectedTags=['task_two'])

        self.assertClosedInterval(j[1],
                                  expectedStart=two_hour_thirty_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=['task_one'])

    def test_fill_open_interval(self):
        """Expand open interval to fill gaps"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)


        one_hour_before_utc = now_utc - timedelta(hours=1)
        one_hour_thirty_before_utc = now_utc - timedelta(hours=1, minutes=30)
        two_hour_thirty_before_utc = now_utc - timedelta(hours=2, minutes=30)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_two".format(three_hours_before_utc, two_hour_thirty_before_utc))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z task_one".format(one_hour_thirty_before_utc, one_hour_before_utc))

        code, out, err = self.t("fill @1")

        self.assertIn('Backfilled', out)
        self.assertNotIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 2)

        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=two_hour_thirty_before_utc,
                                  expectedTags=['task_two'])

        self.assertOpenInterval(j[1],
                                  expectedStart=two_hour_thirty_before_utc,
                                  expectedTags=['task_one'])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
