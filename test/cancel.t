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


class TestCancel(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_cancel_inactive_time_tracking(self):
        """Verify cancelling inactive time tracking"""
        now_utc = datetime.now().utcnow()

        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track foo {:%Y-%m-%dT%H:%M:%SZ} - {:%Y-%m-%dT%H:%M:%SZ}".format(one_hour_before_utc, now_utc))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart=one_hour_before_utc,
                                  expectedEnd=now_utc,
                                  expectedTags=["foo"])

        code, out, err = self.t("cancel")

        self.assertIn("There is no active time tracking.", out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart=one_hour_before_utc,
                                  expectedEnd=now_utc,
                                  expectedTags=["foo"])

    def test_cancel_active_time_tracking(self):
        """Verify cancelling active time tracking"""
        now_utc = datetime.now().utcnow()

        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start foo {:%Y-%m-%dT%H:%M:%SZ}".format(one_hour_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0],
                                expectedStart=one_hour_before_utc,
                                expectedTags=["foo"])

        code, out, err = self.t("cancel")

        self.assertIn("Canceled active time tracking.", out)

        j = self.t.export()

        self.assertEqual(len(j), 0)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

