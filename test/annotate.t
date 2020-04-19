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


class TestAnnotate(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_add_annotation_to_open_interval(self):
        """Add an annotation to an open interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        code, out, err = self.t("annotate @1 foo")

        self.assertIn("Annotated @1 with \"foo\"", out)

        j = self.t.export()
        self.assertOpenInterval(j[0], expectedAnnotation="foo")

    def test_should_use_default_on_missing_id_and_active_time_tracking(self):
        """Use open interval when adding annotation with missing id and active time tracking"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z bar".format(one_hour_before_utc))

        code, out, err = self.t("annotate foo")

        self.assertIn("Annotated @1 with \"foo\"", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedAnnotation="")
        self.assertOpenInterval(j[1], expectedAnnotation="foo")

    def test_should_fail_on_missing_id_and_empty_database(self):
        """Adding annotation with missing id on empty database is an error"""
        code, out, err = self.t.runError("annotate foo")

        self.assertIn("There is no active time tracking.", err)

    def test_should_fail_on_missing_id_and_inactive_time_tracking(self):
        """Adding annotation with missing id on inactive time tracking is an error"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t.runError("annotate foo")

        self.assertIn("At least one ID must be specified.", err)

    def test_remove_annotation_from_interval(self):
        """Calling 'annotate' without annotation removes annotation"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("annotate @1")

        self.assertIn("Removed annotation from @1", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedAnnotation="")

    def test_add_annotation_to_closed_interval(self):
        """Add an annotation to a closed interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("annotate @1 foo")

        self.assertIn("Annotated @1 with \"foo\"", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedAnnotation="foo")

    def test_add_annotation_to_multiple_intervals(self):
        """Add an annotation to multiple intervals"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z one".format(two_hours_before_utc, one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z two".format(one_hour_before_utc, now_utc))

        code, out, err = self.t("annotate @1 @2 foo")

        self.assertIn("Annotated @1 with \"foo\"\nAnnotated @2 with \"foo\"", out)

        j = self.t.export()
        self.assertClosedInterval(j[0], expectedAnnotation="foo")
        self.assertClosedInterval(j[1], expectedAnnotation="foo")

    def test_annotate_synthetic_interval(self):
        """Annotate a synthetic interval."""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        self.t("annotate @2 bar")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc),
                                  expectedAnnotation="bar",
                                  description="modified interval")
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedAnnotation="",
                                description="unmodified interval")

    def test_annotate_with_identical_ids(self):
        """Call 'annotate' with identical ids"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))
        self.t("annotate @1 @1 foo")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedAnnotation="foo")

    def test_annotate_with_embedded_quotes(self):
        """Call 'annotate' with embedded quotes"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc, now_utc))
        self.t("annotate @1 'bar \"foo\" bar'")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedAnnotation='bar "foo" bar')

    def test_annotate_synthetic_and_nonsynthetic_intervals(self):
        """timew annotate should be able to work on both synthetic and non-synthetic intervals in the same invocation"""

        now = datetime.now()
        day_before = now - timedelta(days=1)
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        # First, we'll add a closed interval that is outside the exclusion.
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(day_before, day_before + timedelta(hours=1)))
        # Next create an open interval that crosses the exclusion. This will
        # result in the creation of synthetic intervals
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        # Annotate one of the synthetic and the non-synthetic interval
        self.t("annotate @3 @2 bar")

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(day_before),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(day_before + timedelta(hours=1)),
                                  expectedAnnotation="bar",
                                  description="modified interval")
        self.assertClosedInterval(j[1],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc),
                                  expectedAnnotation="bar",
                                  description="modified interval")
        self.assertOpenInterval(j[2],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedAnnotation="",
                                description="unmodified interval")

if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

