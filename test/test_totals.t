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
import subprocess
import sys
import unittest

import datetime

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import TestCase


class TestTotals(TestCase):
    def setUp(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.process = subprocess.Popen([os.path.join(current_dir, '../ext/totals.py')],
                                        stdin=subprocess.PIPE,
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE)

    def test_totals_with_empty_database(self):
        """totals extension should report error on empty database"""
        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start:
temp.report.end:

[
]
""")

        self.assertEqual('There is no data in the database\n', out)
        self.assertEqual('', err)

    def test_totals_with_filled_database(self):
        """totals extension should print report for filled database"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start: {0:%Y%m%dT%H%M%S}Z
temp.report.end: {1:%Y%m%dT%H%M%S}Z

[
{{"start":"{0:%Y%m%dT%H%M%S}Z","end":"{1:%Y%m%dT%H%M%S}Z","tags":["foo"]}}
]
""".format(one_hour_before_utc, now_utc))

        self.assertRegexpMatches(out, """
Total by Tag, for {0:%Y-%m-%d %H:%M}:\d{{2}} - {1:%Y-%m-%d %H:%M}:\d{{2}}

Tag        Total
----- ----------
foo      1:00:0[01]
      ----------
Total    1:00:0[01]
""".format(one_hour_before, now))
        self.assertEqual('', err)

    def test_totals_with_interval_without_tags(self):
        """totals extension should handle interval without tags"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start: {0:%Y%m%dT%H%M%S}Z
temp.report.end: {1:%Y%m%dT%H%M%S}Z

[
{{"start":"{0:%Y%m%dT%H%M%S}Z","end":"{1:%Y%m%dT%H%M%S}Z"}}
]
""".format(one_hour_before_utc, now_utc))

        self.assertRegexpMatches(out, """
Total by Tag, for {0:%Y-%m-%d %H:%M}:\d{{2}} - {1:%Y-%m-%d %H:%M}:\d{{2}}

Tag        Total
----- ----------
         1:00:0[01]
      ----------
Total    1:00:0[01]
""".format(one_hour_before, now))
        self.assertEqual('', err)

    def test_totals_with_interval_with_empty_tag_list(self):
        """totals extension should handle interval with empty tag list"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start: {0:%Y%m%dT%H%M%S}Z
temp.report.end: {1:%Y%m%dT%H%M%S}Z

[
{{"start":"{0:%Y%m%dT%H%M%S}Z","end":"{1:%Y%m%dT%H%M%S}Z", "tags":[]}}
]
""".format(one_hour_before_utc, now_utc))

        self.assertRegexpMatches(out, """
Total by Tag, for {0:%Y-%m-%d %H:%M}:\d{{2}} - {1:%Y-%m-%d %H:%M}:\d{{2}}

Tag        Total
----- ----------
         1:00:0[01]
      ----------
Total    1:00:0[01]
""".format(one_hour_before, now))
        self.assertEqual('', err)

    def test_totals_with_open_interval(self):
        """totals extension should handle open interval"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start: {0:%Y%m%dT%H%M%S}Z
temp.report.end: 

[
{{"start":"{0:%Y%m%dT%H%M%S}Z","tags":["foo"]}}
]
""".format(one_hour_before_utc))

        self.assertRegexpMatches(out, """
Total by Tag, for {0:%Y-%m-%d %H:%M}:\d{{2}} - {1:%Y-%m-%d %H:%M}:\d{{2}}

Tag        Total
----- ----------
foo      1:00:0[01]
      ----------
Total    1:00:0[01]
""".format(one_hour_before, now))
        self.assertEqual('', err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
