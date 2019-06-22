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


class TestLengthen(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_lengthen_closed_interval(self):
        """Lengthen a closed interval"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")
        code, out, err = self.t("lengthen @1 10mins")
        self.assertIn('Lengthened @1 by 0:10:00', out)

    def test_lengthen_open_interval(self):
        """Lengthen an open interval"""
        self.t("start 30mins ago foo")
        code, out, err = self.t.runError("lengthen @1 10mins")
        self.assertIn('Cannot lengthen open interval @1', err)

    def test_lengthen_synthetic_interval(self):
        """Lengthen a synthetic interval."""
        now = datetime.now()
        now_utc = now.utcnow()

        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        self.t("start {:%Y-%m-%dT%H:%M:%S}".format(five_hours_before))

        self.t("lengthen @2 5min")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc + timedelta(minutes=5)),
                                  expectedTags=[],
                                  description="lengthened interval")
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedTags=[],
                                description="unmodified interval")

    def test_lengthen_an_interval_to_enclose_a_month_border(self):
        """Lengthen an interval to enclose a month border"""
        self.t("track 20180831T220000 - 20180831T230000 foo")
        self.t("lengthen @1 4h")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0])

    # TODO Add :adjust tests.


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
