#!/usr/bin/env python3

###############################################################################
#
# Copyright 2023, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

    def test_should_use_default_on_missing_id_and_active_time_tracking(self):
        """Use open interval when retagging with missing id and active time tracking"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z bar".format(one_hour_before_utc))

        code, out, err = self.t("retag baz")

        self.assertIn("Retagged @1 as baz", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertOpenInterval(j[1], expectedTags=["baz"])

    def test_should_fail_on_missing_id_and_empty_database(self):
        """Retagging interval with missing id on empty database is an error"""
        code, out, err = self.t.runError("retag foo")

        self.assertIn("There is no active time tracking.", err)

    def test_should_fail_on_missing_id_and_inactive_time_tracking(self):
        """Retagging with missing id on inactive time tracking is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t.runError("retag foo")

        self.assertIn("At least one ID must be specified.", err)

    def test_should_fail_on_no_tags(self):
        """Calling command 'retag' without tags is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t.runError("retag @1")

        self.assertIn("At least one tag must be specified.", err)

    def test_retag_tagless_closed_interval_with_single_tag(self):
        """Retag a tagless, closed interval with a single tag"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("retag @1 foo")

        self.assertIn("Retagged @1 as foo", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_retag_tagless_closed_interval_with_multiple_tags(self):
        """Retag a tagless, closed interval with multiple tags"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("retag @1 foo bar")

        self.assertIn("Retagged @1 as bar foo", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "foo"])

    def test_retag_tagless_open_interval_with_single_tag(self):
        """Retag a tagless, open interval with a single tag"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        code, out, err = self.t("retag @1 foo")

        self.assertIn("Retagged @1 as foo", out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["foo"])

    def test_retag_tagless_open_interval_with_multiple_tags(self):
        """Retag a tagless, open interval with multiple tags"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        code, out, err = self.t("retag @1 foo bar")

        self.assertIn("Retagged @1 as bar foo", out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["bar", "foo"])

    def test_retag_tagged_open_interval_with_single_tag(self):
        """Retag a tagged, open interval with a single tag"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(one_hour_before_utc))

        code, out, err = self.t("retag @1 bar")

        self.assertIn("Retagged @1 as bar", out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["bar"])

    def test_retag_tagged_open_interval_with_multiple_tags(self):
        """Retag a tagged, open interval with a single tag"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(one_hour_before_utc))

        code, out, err = self.t("retag @1 bar buz")

        self.assertIn("Retagged @1 as bar buz", out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedTags=["bar", "buz"])

    def test_retag_tagged_closed_interval_with_single_tag(self):
        """Retag a tagged, closed interval with a single tag"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("retag @1 bar")

        self.assertIn("Retagged @1 as bar", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar"])

    def test_retag_tagged_closed_interval_with_multiple_tags(self):
        """Retag a tagged, closed interval with multiple tag"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo bar".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("retag @1 buz fuz")

        self.assertIn("Retagged @1 as buz fuz", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["buz", "fuz"])

    def test_retag_multiple_intervals_with_single_tag(self):
        """Retag multiple intervals with a single tag"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z one".format(two_hours_before_utc, one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z two".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("retag @1 @2 foo")

        self.assertIn("Retagged @2 as foo\nRetagged @1 as foo", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["foo"])
        self.assertClosedInterval(j[1], expectedTags=["foo"])

    def test_retag_multiple_intervals_with_multiple_tags(self):
        """Retag multiple intervals with multiple tags"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z one".format(two_hours_before_utc, one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z two".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("retag @1 @2 foo bar")

        self.assertIn("Retagged @2 as bar foo\nRetagged @1 as bar foo", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedTags=["bar", "foo"])
        self.assertClosedInterval(j[1], expectedTags=["bar", "foo"])

    def test_retag_synthetic_interval(self):
        """Retag a synthetic interval."""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        self.t("retag @2 bar")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc),
                                  expectedTags=["bar"],
                                  description="modified interval")
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedTags=["foo"],
                                description="unmodified interval")

    def test_retag_with_identical_ids(self):
        """Call 'retag' with identical ids"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))
        self.t("tag @1 @1 foo")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_retag_with_new_tag(self):
        """Call 'retag' with new tag"""
        now_utc = datetime.now().utcnow()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} foo".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("retag @1 bar")

        self.assertIn("Note: 'bar' is a new tag", out)
        self.assertIn("Retagged @1 as bar", out)

    def test_retag_with_previous_tag(self):
        """Call 'retag' with previous tag"""
        now_utc = datetime.now().utcnow()

        three_hours_before_utc = now_utc - timedelta(hours=3)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} bar".format(three_hours_before_utc, two_hours_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S} - {:%Y-%m-%dT%H:%M:%S} foo".format(two_hours_before_utc, one_hour_before_utc))
        code, out, err = self.t("retag @1 bar")

        self.assertNotIn("Note: 'bar' is a new tag", out)
        self.assertIn("Retagged @1 as bar", out)

    def test_retag_with_percent_sign(self):
        """Call 'retag' with an embedded percent sign"""
        self.t("start 1h ago bar")
        code, out, err = self.t("retag @1 reta%g")

        self.assertIn("Note: '\"reta%g\"' is a new tag", out)

        self.t("stop")
        self.t("delete @1")

    def test_retag_with_double_quote(self):
        """Call 'retag' with an embedded double quote sign"""
        self.t("start 1h ago bar")
        code, out, err = self.t("retag @1 'this is a \"test\"'")

        self.assertIn("Note: '\"this is a \\\"test\\\"\"' is a new tag", out)
        self.t("stop")
        self.t("delete @1")

    def test_referencing_a_non_existent_interval_is_an_error(self):
        """Calling retag with a non-existent interval reference is an error"""
        code, out, err = self.t.runError("retag @1 @2 foo")
        self.assertIn("ID '@1' does not correspond to any tracking.", err)

        self.t("start 1h ago bar")

        code, out, err = self.t.runError("retag @2 foo")
        self.assertIn("ID '@2' does not correspond to any tracking.", err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
