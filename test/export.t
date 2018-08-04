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

from datetime import datetime, timedelta

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
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(one_hour_before_utc, now_utc))

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart=one_hour_before_utc,
                                  expectedEnd=now_utc,
                                  expectedTags=["foo"])

    def test_changing_exclusion_does_not_change_flattened_intervals(self):
        """Changing exclusions does not change flattened intervals"""
        self.t.configure_exclusions("12:22:44", "13:32:23")

        self.t("track 20160101T102255Z - 20160101T154422Z foo")

        self.t.configure_exclusions("12:44:22", "13:23:32")

        self.t("track 20160102T102255Z - 20160102T154422Z bar")

        j = self.t.export()

        self.assertEqual(len(j), 4)
        self.assertClosedInterval(j[0],
                                  expectedStart="20160101T102255Z",
                                  expectedEnd="20160101T112244Z",
                                  expectedTags=["foo"])
        self.assertClosedInterval(j[1],
                                  expectedStart="20160101T123223Z",
                                  expectedEnd="20160101T154422Z",
                                  expectedTags=["foo"])
        self.assertClosedInterval(j[2],
                                  expectedStart="20160102T102255Z",
                                  expectedEnd="20160102T114422Z",
                                  expectedTags=["bar"])
        self.assertClosedInterval(j[3],
                                  expectedStart="20160102T122332Z",
                                  expectedEnd="20160102T154422Z",
                                  expectedTags=["bar"])

    def test_changing_exclusion_does_change_open_interval(self):
        """Changing exclusions does change open interval"""
        now = datetime.now()
        now_utc = now.utcnow()

        two_hours_before = now - timedelta(hours=2)
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions(four_hours_before.time(), three_hours_before.time())

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 2)

        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc),
                                  expectedTags=["foo"])
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedTags=["foo"])

        self.t.configure_exclusions(three_hours_before.time(), two_hours_before.time())

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                  expectedTags=["foo"])
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(two_hours_before_utc),
                                expectedTags=["foo"])

    def test_export_with_tag_with_spaces(self):
        """Interval with tag with spaces"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z \"tag with spaces\"".format(one_hour_before_utc, now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["tag with spaces"])

    def test_export_with_tag_with_quote(self):
        """Interval with tag with quote"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z \"tag with \\\"quote\"".format(one_hour_before_utc, now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["tag with \"quote"])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
