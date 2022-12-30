#!/usr/bin/env python3
import json
import os
import sys
import unittest
from datetime import datetime, timedelta

###############################################################################
#
# Copyright 2016 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestExtensions(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_cli(self):
        self.t.add_default_extension('ext_echo')

        code, out, err = self.t('ext_echo')
        self.assertIn('test works', out)

        code, out, err = self.t('ext_')
        self.assertIn('test works', out)

        code, out, err = self.t('report ext_echo')
        self.assertIn('test works', out)

        code, out, err = self.t('report ext')
        self.assertIn('test works', out)

    def test_default_range_is_applied_when_no_range_given_on_command_line(self):
        """Default range is applied when no range is given on the command line"""
        self.t.add_default_extension("debug.py")

        self.t.config("reports.debug.range", "':day'")

        now = datetime.now()
        now_utc = now.utcnow()

        yesterday_one_hour_before_utc = now_utc - timedelta(hours=1, days=1)
        yesterday_two_hours_before_utc = now_utc - timedelta(hours=2, days=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(yesterday_two_hours_before_utc, yesterday_one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z bar".format(now_utc))

        code, out, err = self.t("debug")

        j = json.loads(out)

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(now_utc),
                                  expectedTags=["bar"])

    def test_default_range_is_overridden_when_range_given_on_command_line(self):
        """Default range is overridden when range is given on the command line"""
        self.t.add_default_extension("debug.py")

        self.t.config("reports.debug.range", "':day'")

        now = datetime.now()
        now_utc = now.utcnow()

        yesterday_one_hour_before_utc = now_utc - timedelta(hours=1, days=1)
        yesterday_two_hours_before_utc = now_utc - timedelta(hours=2, days=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z foo".format(yesterday_two_hours_before_utc, yesterday_one_hour_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z bar".format(now_utc))

        code, out, err = self.t("debug :all")

        j = json.loads(out)

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="{:%Y%m%dT%H%M%S}Z".format(yesterday_two_hours_before_utc),
                                  expectedEnd="{:%Y%m%dT%H%M%S}Z".format(yesterday_one_hour_before_utc),
                                  expectedTags=["foo"])
        self.assertOpenInterval(j[1],
                                expectedStart="{:%Y%m%dT%H%M%S}Z".format(now_utc),
                                expectedTags=["bar"])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
