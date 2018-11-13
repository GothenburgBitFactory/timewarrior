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

class TestModify(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_modify_end_of_open_interval(self):
        """Attempt to modify end of an open interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))
        code, out, err = self.t.runError("modify end @1 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))
        self.assertIn("Cannot modify end of open interval", err)

    def test_modify_start_of_open_interval(self):
        """Modify start of open interval"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))
        code, out, err = self.t("modify start @1 {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        j = self.t.export()
        self.assertEquals(len(j), 1)
        self.assertOpenInterval(j[0],
            expectedStart="{:%Y%m%dT%H%M%S}Z".format(one_hour_before_utc))

    def test_modify_invalid_subcommand(self):
        """Modify without (start|stop) subcommand"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))
        self.t("stop")
        code, out, err = self.t.runError("modify @1 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))
        self.assertIn("Must specify start|end command to modify", err)

    def test_modify_no_end_time(self):
        """Modify without a time to stop at"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))
        self.t("stop")
        code, out, err = self.t.runError("modify end @1")
        self.assertIn("No updated time", err)

    def test_modify_shorten_one_hour(self):
        """Shorten the interval by one hour."""
        now_utc = datetime.now().utcnow()

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t("modify end @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=2)))

        j = self.t.export()
        self.assertEquals(len(j), 2)
        self.assertClosedInterval(j[0],
            expectedStart="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=3)),
            expectedEnd="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=2)))

    def test_modify_shorten_before_start(self):
        """Modify should not move end before start."""
        now_utc = datetime.now().utcnow()

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify end @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=4)))
        self.assertIn("Cannot modify interval", err);

    def test_modify_start_to_after_end(self):
        """Modify should not move start beyond end."""
        now_utc = datetime.now().utcnow()

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify start @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=59)))
        self.assertIn("Cannot modify interval", err);

    def test_modify_start_within_interval(self):
        """Increase start time within interval."""
        now_utc = datetime.now().utcnow()

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t("modify start @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=2)))

        j = self.t.export()
        self.assertEquals(len(j), 2)
        self.assertClosedInterval(j[0],
            expectedStart="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=2)),
            expectedEnd="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=1)))

    def test_modify_move_stop_to_overlap_following_interval(self):
        """Move end time to overlap with following interval."""
        now_utc = datetime.now().utcnow()

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify end @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=30)))
        self.assertIn("You cannot overlap intervals", err)

    def test_modify_move_start_to_overlap_preceeding_interval(self):
        """Move start time to overlap with preceeding interval."""
        now_utc = datetime.now().utcnow()

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify start @1 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=2)))
        self.assertIn("You cannot overlap intervals", err)

if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
