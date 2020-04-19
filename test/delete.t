#!/usr/bin/env python3

###############################################################################
#
# Copyright 2016 - 2019, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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


class TestDelete(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_delete_open(self):
        """Delete a single open interval"""
        self.t("track :yesterday foo")
        self.t("start today bar")

        code, out, err = self.t("delete @1")
        self.assertEqual(out, 'Deleted @1\n')

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["foo"])

    def test_delete_closed(self):
        """Delete a single closed interval"""
        self.t("track :yesterday foo")
        self.t("start today bar")
        code, out, err = self.t("delete @2")
        self.assertEqual(out, 'Deleted @2\n')

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["bar"])

    def test_delete_multiple(self):
        """Delete a mix of open/closed intervals"""
        self.t("track :yesterday foo")
        self.t("start today bar")
        code, out, err = self.t("delete @1 @2")
        self.assertIn('Deleted @1', out)
        self.assertIn('Deleted @2', out)

        j = self.t.export()
        self.assertEqual(len(j), 0)

    def test_delete_open_interval_spanning_exclusion(self):
        """Delete an open interval that spans over an exclusion"""
        now = datetime.now()
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.utcnow()
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions([(four_hours_before.time(), three_hours_before.time())])

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        # Delete the open interval.
        code, out, err = self.t("delete @1")  # self.t("delete @1 :debug")
        self.assertEqual(out, 'Deleted @1\n')

        # The last interval should be closed, because the open interval was deleted.
        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart='{:%Y%m%dT%H%M%S}Z'.format(five_hours_before_utc),
                                  expectedEnd='{:%Y%m%dT%H%M%S}Z'.format(four_hours_before_utc),
                                  expectedTags=['foo'],
                                  description='remaining interval')

    def test_delete_interval_which_encloses_month_border(self):
        """Delete an interval which encloses a month border"""
        self.t("track 20180831T220000 - 20180901T030000 foo")
        self.t("delete @1")

        j = self.t.export()

        self.assertEqual(len(j), 0)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
