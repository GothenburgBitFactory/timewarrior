#!/usr/bin/env python3

###############################################################################
#
# Copyright 2018 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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


class TestUntag(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_remove_tag_from_open_interval(self):
        """Remove a tag from an open interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo bar baz".format(one_hour_before_utc))

        code, out, err = self.t("untag @1 foo")

        self.assertIn('Removed foo from @1', out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["bar", "baz"])

    def test_should_use_default_on_missing_id_and_active_time_tracking(self):
        """Use open interval when removing tags with missing id and active time tracking"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo baz".format(two_hours_before_utc, one_hour_before_utc))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z bar baz".format(one_hour_before_utc))

        code, out, err = self.t("untag baz")

        self.assertIn("Removed baz from @1", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo", "baz"])
        self.assertOpenInterval(j[1], expectedTags=["bar"])

    def test_should_fail_on_missing_id_and_empty_database(self):
        """Removing tag with missing id on empty database is an error"""
        code, out, err = self.t.runError("untag foo")

        self.assertIn("There is no active time tracking.", err)

    def test_should_fail_on_missing_id_and_inactive_time_tracking(self):
        """Removing tag with missing id on inactive time tracking is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t.runError("untag foo")

        self.assertIn("At least one ID must be specified.", err)

    def test_should_fail_on_no_tags(self):
        """Calling command 'untag' without tags is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t.runError("untag @1")

        self.assertIn("At least one tag must be specified.", err)

    def test_remove_tag_from_closed_interval(self):
        """Remove a tag from a closed interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo bar baz".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("untag @1 foo")

        self.assertIn('Removed foo from @1', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "baz"])

    def test_remove_tags_from_open_interval(self):
        """Remove tags from an open interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo bar baz".format(one_hour_before_utc))

        code, out, err = self.t("untag @1 foo bar")

        self.assertIn('Removed foo bar from @1', out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["baz"])

    def test_remove_tags_from_closed_interval(self):
        """Remove tags from an closed interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo bar baz".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("untag @1 foo bar")

        self.assertIn('Removed foo bar from @1', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["baz"])

    def test_remove_tag_from_multiple_intervals(self):
        """Remove a tag from multiple intervals"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo bar one".format(two_hours_before_utc, one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo bar two".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("untag @1 @2 foo")

        self.assertIn('Removed foo from @1\nRemoved foo from @2', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "one"])
        self.assertClosedInterval(j[1], expectedTags=["bar", "two"])

    def test_remove_tags_from_multiple_intervals(self):
        """Remove tags from multiple intervals"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z one".format(two_hours_before_utc, one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z two".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("untag @1 @2 foo bar")

        self.assertIn('Removed foo bar from @1\nRemoved foo bar from @2', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["one"])
        self.assertClosedInterval(j[1], expectedTags=["two"])

    def test_untag_synthetic_interval(self):
        """Untag a synthetic interval."""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo bar".format(five_hours_before_utc))

        self.t("untag @2 foo")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc),
                                  expectedTags=["bar"],
                                  description="modified interval")
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedTags=["bar", "foo"],
                                description="unmodified interval")

    def test_untag_with_identical_ids(self):
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo bar".format(one_hour_before_utc, now_utc))
        self.t("untag @1 @1 foo")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["bar"])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
