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


class TestTag(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_add_tag_to_open_interval(self):
        """Add a tag to an open interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        code, out, err = self.t("tag @1 foo")

        self.assertIn("Added foo to @1", out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["foo"])

    def test_should_use_default_on_missing_id_and_active_time_tracking(self):
        """Use open interval when adding tags with missing id and active time tracking"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z bar".format(one_hour_before_utc))

        code, out, err = self.t("tag baz")

        self.assertIn("Added baz to @1", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertOpenInterval(j[1], expectedTags=["bar", "baz"])

    def test_should_fail_on_missing_id_and_empty_database(self):
        """Adding tag with missing id on empty database is an error"""
        code, out, err = self.t.runError("tag foo")

        self.assertIn("There is no active time tracking.", err)

    def test_should_fail_on_missing_id_and_inactive_time_tracking(self):
        """Adding tag with missing id on inactive time tracking is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t.runError("tag foo")

        self.assertIn("At least one ID must be specified.", err)

    def test_should_fail_on_no_tags(self):
        """Calling command 'tag' without tags is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t.runError("tag @1")

        self.assertIn("At least one tag must be specified.", err)

    def test_add_tag_to_closed_interval(self):
        """Add a tag to an closed interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("tag @1 foo")

        self.assertIn("Added foo to @1", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_add_tags_to_open_interval(self):
        """Add tags to an open interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        code, out, err = self.t("tag @1 foo bar")

        self.assertIn("Added foo bar to @1", out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["bar", "foo"])

    def test_add_tags_to_closed_interval(self):
        """Add tags to an closed interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("tag @1 foo bar")

        self.assertIn("Added foo bar to @1", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "foo"])

    def test_add_tag_to_multiple_intervals(self):
        """Add a tag to multiple intervals"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z one".format(two_hours_before_utc, one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z two".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("tag @1 @2 foo")

        self.assertIn("Added foo to @1\nAdded foo to @2", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo", "one"])
        self.assertClosedInterval(j[1], expectedTags=["foo", "two"])

    def test_add_tags_to_multiple_intervals(self):
        """Add tags to multiple intervals"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z one".format(two_hours_before_utc, one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z two".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("tag @1 @2 foo bar")

        self.assertIn("Added foo bar to @1\nAdded foo bar to @2", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "foo", "one"])
        self.assertClosedInterval(j[1], expectedTags=["bar", "foo", "two"])

    def test_tag_synthetic_interval(self):
        """Tag a synthetic interval."""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        self.t("tag @2 bar")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc),
                                  expectedTags=["bar", "foo"],
                                  description="modified interval")
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedTags=["foo"],
                                description="unmodified interval")

    def test_tag_with_identical_ids(self):
        """Call 'tag' with identical ids"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))
        self.t("tag @1 @1 foo")

        j = self.t.export()

        self.assertEquals(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_tag_with_new_tag(self):
        """Call 'tag' with new tag"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} foo".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("tag @1 bar")

        self.assertIn("Note: 'bar' is a new tag", out)
        self.assertIn("Added bar to @1", out)

    def test_tag_with_previous_tag(self):
        """Call 'tag' with previous tag"""
        now_utc = datetime.now().utcnow()

        three_hours_before_utc = now_utc - timedelta(hours=3)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} bar".format(three_hours_before_utc, two_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} foo".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("tag @1 bar")

        self.assertNotIn("Note: 'bar' is a new tag", out)
        self.assertIn("Added bar to @1", out)

    def test_tag_with_percent_sign(self):
        """Call 'tag' with an embedded percent sign"""
        code, out, err = self.t("start 'ta%g'")
        self.assertIn("Note: '\"ta%g\"' is a new tag", out)
        self.t("stop")
        self.t("delete @1")


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
