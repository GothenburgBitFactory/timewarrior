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


class TestTag(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_add_tag_to_open_interval(self):
        """Add a tag to an open interval"""
        self.t("start 30min ago")
        code, out, err = self.t("tag @1 foo")
        self.assertIn('Added foo to @1', out)


    def test_should_fail_on_missing_id_and_inactive_time_tracking(self):
        """Missing id on inactive time tracking is an error"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t.runError("tag foo")
        self.assertIn("At least one ID must be specified.", err)

    def test_should_fail_on_no_tags(self):
        """No tags is an error"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t.runError("tag @1")
        self.assertIn("At least one tag must be specified.", err)

    def test_add_tag_to_closed_interval(self):
        """Add a tag to an closed interval"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t("tag @1 foo")
        self.assertIn('Added foo to @1', out)

    def test_add_tags_to_open_interval(self):
        """Add tags to an open interval"""
        self.t("start 30min ago")
        code, out, err = self.t("tag @1 foo bar")
        self.assertIn('Added foo bar to @1', out)

    def test_add_tags_to_closed_interval(self):
        """Add tags to an closed interval"""
        self.t("track yesterday for 1hour")
        code, out, err = self.t("tag @1 foo bar")
        self.assertIn('Added foo bar to @1', out)

    def test_add_tag_to_multiple_intervals(self):
        """Add a tag to multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00")
        code, out, err = self.t("tag @1 @2 foo")
        self.assertIn('Added foo to @1\nAdded foo to @2', out)

    def test_add_tags_to_multiple_intervals(self):
        """Add tags to multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00")
        code, out, err = self.t("tag @1 @2 foo bar")
        self.assertIn('Added foo bar to @1\nAdded foo bar to @2', out)

    def test_tag_synthetic_interval(self):
        """Tag a synthetic interval."""
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

        self.t("start {:%Y-%m-%dT%H}:45:00 foo".format(five_hours_before))

        self.t("tag @2 bar")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}4500Z'.format(now_utc-timedelta(hours=5)), 'start time of modified interval does not match')
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}0000Z'.format(now_utc - timedelta(hours=4)), 'end time of modified interval does not match')
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'], ['bar', 'foo'], 'tags of modified interval do not match')
        self.assertTrue('start' in j[1])
        self.assertEqual(j[1]['start'], '{:%Y%m%dT%H}0000Z'.format(now_utc - timedelta(hours=3)), 'start time of unmodified interval does not match')
        self.assertFalse('end' in j[1])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'], ['foo'], 'tags of unmodified interval do not match')

    def test_tag_with_identical_ids(self):
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00")
        self.t("tag @1 @1 foo")

        j = self.t.export()

        self.assertEquals(len(j), 1)
        self.assertEqual(j[0]['tags'], ['foo'])


class TestUntag(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_remove_tag_from_open_interval(self):
        """Remove a tag from an open interval"""
        self.t("start 30min ago foo bar baz")
        code, out, err = self.t("untag @1 foo")
        self.assertIn('Removed foo from @1', out)

    def test_remove_tag_from_closed_interval(self):
        """Remove a tag from an closed interval"""
        self.t("track yesterday for 1hour foo bar baz")
        code, out, err = self.t("untag @1 foo")
        self.assertIn('Removed foo from @1', out)

    def test_remove_tags_from_open_interval(self):
        """Remove tags from an open interval"""
        self.t("start 30min ago foo bar baz")
        code, out, err = self.t("untag @1 foo bar")
        self.assertIn('Removed foo bar from @1', out)

    def test_remove_tags_from_closed_interval(self):
        """Remove tags from an closed interval"""
        self.t("track yesterday for 1hour foo bar baz")
        code, out, err = self.t("untag @1 foo bar")
        self.assertIn('Removed foo bar from @1', out)

    def test_remove_tag_from_multiple_intervals(self):
        """Remove a tag from multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar baz")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00 foo bar baz")
        code, out, err = self.t("untag @1 @2 foo")
        self.assertIn('Removed foo from @1\nRemoved foo from @2', out)

    def test_remove_tags_from_multiple_intervals(self):
        """Remove tags from multiple intervals"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar baz")
        self.t("track 2016-01-01T01:00:00 - 2016-01-01T02:00:00 foo bar baz")
        code, out, err = self.t("untag @1 @2 foo bar")
        self.assertIn('Removed foo bar from @1\nRemoved foo bar from @2', out)

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
        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}4500Z'.format(now_utc-timedelta(hours=5)), 'start time of modified interval does not match')
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}0000Z'.format(now_utc - timedelta(hours=4)), 'end time of modified interval does not match')
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'], ['bar'], 'tags of modified interval do not match')
        self.assertTrue('start' in j[1])
        self.assertEqual(j[1]['start'], '{:%Y%m%dT%H}0000Z'.format(now_utc - timedelta(hours=3)), 'start time of unmodified interval does not match')
        self.assertFalse('end' in j[1])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'], ['bar', 'foo'], 'tags of unmodified interval do not match')

    def test_untag_with_identical_ids(self):
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo bar")
        self.t("untag @1 @1 foo")

        j = self.t.export()

        self.assertEquals(len(j), 1)
        self.assertEqual(j[0]['tags'], ['bar'])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
