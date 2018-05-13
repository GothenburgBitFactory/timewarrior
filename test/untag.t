#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2006 - 2018, Paul Beckingham, Federico Hernandez.
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


class TestUntag(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_remove_tag_from_open_interval(self):
        """Remove a tag from an open interval"""
        self.t("start 30min ago foo bar baz")
        code, out, err = self.t("untag @1 foo")
        self.assertIn('Removed foo from @1', out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["bar", "baz"])

    def test_should_use_default_on_missing_id_and_active_time_tracking(self):
        """Use open interval when removing tags with missing id and active time tracking"""
        self.t("track yesterday for 1hour foo")
        self.t("start 30min ago bar baz")
        code, out, err = self.t("untag baz")
        self.assertIn("Removed baz from @1", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertOpenInterval(j[1], expectedTags=["bar"])

    def test_should_fail_on_missing_id_and_empty_database(self):
        """Removing tag with missing id on empty database is an error"""
        code, out, err = self.t.runError("untag foo")
        self.assertIn("There is no active time tracking.", err)

    def test_should_fail_on_missing_id_and_inactive_time_tracking(self):
        """Removing tag with missing id on inactive time tracking is an error"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t.runError("untag foo")
        self.assertIn("At least one ID must be specified.", err)

    def test_should_fail_on_no_tags(self):
        """Calling command 'untag' without tags is an error"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t.runError("untag @1")
        self.assertIn("At least one tag must be specified.", err)

    def test_remove_tag_from_closed_interval(self):
        """Remove a tag from a closed interval"""
        self.t("track yesterday for 1hour foo bar baz")
        code, out, err = self.t("untag @1 foo")
        self.assertIn('Removed foo from @1', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "baz"])

    def test_remove_tags_from_open_interval(self):
        """Remove tags from an open interval"""
        self.t("start 30min ago foo bar baz")
        code, out, err = self.t("untag @1 foo bar")
        self.assertIn('Removed foo bar from @1', out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["baz"])

    def test_remove_tags_from_closed_interval(self):
        """Remove tags from an closed interval"""
        self.t("track yesterday for 1hour foo bar baz")
        code, out, err = self.t("untag @1 foo bar")
        self.assertIn('Removed foo bar from @1', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["baz"])

    def test_remove_tag_from_multiple_intervals(self):
        """Remove a tag from multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar one")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00 foo bar two")
        code, out, err = self.t("untag @1 @2 foo")
        self.assertIn('Removed foo from @1\nRemoved foo from @2', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "one"])
        self.assertClosedInterval(j[1], expectedTags=["bar", "two"])

    def test_remove_tags_from_multiple_intervals(self):
        """Remove tags from multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar one")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00 foo bar two")
        code, out, err = self.t("untag @1 @2 foo bar")
        self.assertIn('Removed foo bar from @1\nRemoved foo bar from @2', out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["one"])
        self.assertClosedInterval(j[1], expectedTags=["two"])

    def test_untag_synthetic_interval(self):
        """Untag a synthetic interval."""
        now = datetime.now()
        now_utc = now.utcnow()

        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        if four_hours_before.day < three_hours_before.day:
            exclusion = "<{:%H}:00 >{:%H}:00".format(three_hours_before, four_hours_before)
        else:
            exclusion = "{:%H}:00-{:%H}:00".format(four_hours_before, three_hours_before)

        self.t.config("exclusions.friday", exclusion)
        self.t.config("exclusions.thursday", exclusion)
        self.t.config("exclusions.wednesday", exclusion)
        self.t.config("exclusions.tuesday", exclusion)
        self.t.config("exclusions.monday", exclusion)
        self.t.config("exclusions.sunday", exclusion)
        self.t.config("exclusions.saturday", exclusion)

        self.t("start {:%Y-%m-%dT%H}:45:00 foo bar".format(five_hours_before))

        self.t("untag @2 foo")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H}4500Z".format(now_utc - timedelta(hours=5)),
                                  expectedEnd="{:%Y%m%dT%H}0000Z".format(now_utc - timedelta(hours=4)),
                                  expectedTags=["bar"],
                                  description="modified interval")
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H}0000Z".format(now_utc - timedelta(hours=3)),
                                expectedTags=["bar", "foo"],
                                description="unmodified interval")

    def test_untag_with_identical_ids(self):
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar")
        self.t("untag @1 @1 foo")

        j = self.t.export()

        self.assertEquals(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["bar"])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
