#!/usr/bin/env python2
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

from datetime import datetime, timedelta, time

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestShorten(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_shorten_closed_interval(self):
        """Shorten a closed interval"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")

        code, out, err = self.t("shorten @1 10mins")

        self.assertIn('Shortened @1 by 0:10:00', out)

    def test_shorten_open_interval(self):
        """Shorten an open interval"""
        self.t("start 30mins ago foo")

        code, out, err = self.t.runError("shorten @1 10mins")

        self.assertIn('Cannot shorten open interval @1', err)

    def test_shorten_interval_moved_into_exclusion(self):
        """Shorten an interval moved to span an exclusion."""
        self.t.configure_exclusions([(time(16, 30, 0), time(7, 30, 0)),
                                     (time(12, 0, 0), time(13, 0, 0))])

        self.t('track 20170308T140000 - 20170308T161500')

        self.t("move @1 20170308T113000")
        self.t("shorten @1 5min")  # Does not work.

    def test_shorten_synthetic_interval(self):
        """Shorten a synthetic interval."""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(five_hours_before_utc))

        self.t("shorten @2 5min")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(five_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(four_hours_before_utc - timedelta(minutes=5)),
                                  expectedTags=[])
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(three_hours_before_utc),
                                expectedTags=[])

    def test_over_shorten_closed_interval(self):
        """Over-shorten interval is an error"""
        self.t("track 2016-06-08T07:30:00 - 2016-06-08T07:35:00 foo")

        code, out, err = self.t.runError("shorten @1 10mins")

        self.assertIn('Cannot shorten interval @1 by 0:10:00 because it is only 0:05:00 in length.', err)

    def test_shorten_closed_interval_to_zero(self):
        """Shorten interval to zero"""
        self.t("track 2016-06-08T07:30:00Z - 2016-06-08T07:35:00Z foo")

        self.t("shorten @1 5mins")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart="20160608T073000Z",
                                  expectedEnd="20160608T073000Z")

    def test_shorten_an_interval_which_encloses_month_border(self):
        """Shorten an interval which encloses a month border"""
        self.t("track 20180831T220000 - 20180901T030000 foo")
        self.t("shorten @1 4h")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
