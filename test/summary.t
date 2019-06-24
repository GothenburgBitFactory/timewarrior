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


class TestSummary(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_empty(self):
        """Summary should print warning if no data in range"""
        code, out, err = self.t("summary")
        self.assertIn("No filtered data found in the range", out)

    def test_filled(self):
        """Summary should be printed if data is available"""
        now = datetime.now()
        yesterday = now - timedelta(days=1)
        tomorrow = now + timedelta(days=1)

        self.t("track for 1h")

        code, out, err = self.t("summary :ids {:%Y-%m-%d} - {:%Y-%m-%d}".format(yesterday, tomorrow))

        self.assertRegexpMatches(out, """
Wk  ?Date       Day ID Tags    ?Start      ?End    Time   Total
[ -]+
W\d{1,2} \d{4}-\d{2}-\d{2} .{3} @1       ?\d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2}(
W\d{1,2} \d{4}-\d{2}-\d{2} .{3} @1       ?\d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2})?

[ ]+1:00:0[01]
""")

    def test_with_open_interval(self):
        """Summary should print open interval"""
        now = datetime.now()
        yesterday = now - timedelta(days=1)
        tomorrow = now + timedelta(days=1)

        self.t("start 1h ago")

        code, out, err = self.t("summary :ids {:%Y-%m-%d} - {:%Y-%m-%d}".format(yesterday, tomorrow))

        self.assertRegexpMatches(out, """
Wk  ?Date       Day ID Tags    ?Start End    Time   Total
[ -]+
W\d{1,2} \d{4}-\d{2}-\d{2} .{3} @1       ?\d{1,2}:\d{2}:\d{2}[ ]+- \d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2}(
W\d{1,2} \d{4}-\d{2}-\d{2} .{3} @1       ?\d{1,2}:\d{2}:\d{2}[ ]+- \d{1,2}:\d{2}:\d{2} \d{1,2}:\d{2}:\d{2})?

[ ]+1:00:0[01]
""")

    def test_with_range_filter(self):
        """Summary should print data filtered by date range"""
        self.t("track Tag1 2017-03-09T08:43:08 - 2017-03-09T09:38:15")
        self.t("track Tag2 2017-03-09T11:38:39 - 2017-03-09T11:45:35")
        self.t("track Tag2 Tag3 2017-03-09T11:46:21 - 2017-03-09T12:00:17")
        self.t("track Tag2 Tag4 2017-03-09T12:01:49 - 2017-03-09T12:28:46")

        code, out, err = self.t("summary 2017-03-09T11:00 - 2017-03-09T12:00 :ids")

        self.assertIn("""
Wk  Date       Day ID Tags          Start      End    Time   Total
--- ---------- --- -- ---------- -------- -------- ------- -------
W10 2017-03-09 Thu @3 Tag2       11:38:39 11:45:35 0:06:56
                   @2 Tag2, Tag3 11:46:21 12:00:17 0:13:56 0:20:52

                                                           0:20:52
""", out)

    def test_with_date_filter(self):
        """Summary should print data filtered by date"""
        self.t("track 2017-03-09T10:00:00 - 2017-03-09T11:00:00")
        self.t("track 2017-03-10T10:00:00 - 2017-03-10T11:00:00")
        self.t("track 2017-03-11T10:00:00 - 2017-03-11T11:00:00")

        code, out, err = self.t("summary 2017-03-10 :ids")

        self.assertIn("""
Wk  Date       Day ID Tags    Start      End    Time   Total
--- ---------- --- -- ---- -------- -------- ------- -------
W10 2017-03-10 Fri @2      10:00:00 11:00:00 1:00:00 1:00:00

                                                     1:00:00
""", out)

    def test_with_tag_filter(self):
        """Summary should print data filtered by tag"""
        self.t("track Tag1 2017-03-09T08:43:08 - 2017-03-09T09:38:15")
        self.t("track Tag2 2017-03-09T11:38:39 - 2017-03-09T11:45:35")
        self.t("track Tag2 Tag3 2017-03-09T11:46:21 - 2017-03-09T12:00:17")
        self.t("track Tag2 Tag4 2017-03-09T12:01:49 - 2017-03-09T12:28:46")

        code, out, err = self.t("summary 2017-03-09 Tag1 :ids")

        self.assertIn("""
Wk  Date       Day ID Tags   Start     End    Time   Total
--- ---------- --- -- ---- ------- ------- ------- -------
W10 2017-03-09 Thu @4 Tag1 8:43:08 9:38:15 0:55:07 0:55:07

                                                   0:55:07
""", out)

    def test_with_day_gap(self):
        """Summary should skip days with no data"""
        self.t("track 2017-03-09T10:00:00 - 2017-03-09T11:00:00")
        self.t("track 2017-03-11T10:00:00 - 2017-03-11T11:00:00")

        code, out, err = self.t("summary 2017-03-09 - 2017-03-12 :ids")

        self.assertIn("""
Wk  Date       Day ID Tags    Start      End    Time   Total
--- ---------- --- -- ---- -------- -------- ------- -------
W10 2017-03-09 Thu @2      10:00:00 11:00:00 1:00:00 1:00:00
W10 2017-03-11 Sat @1      10:00:00 11:00:00 1:00:00 1:00:00

                                                     2:00:00
""", out)

    def test_with_week_change(self):
        """Summary should display week change"""
        self.t("track 2017-03-11T10:00:00 - 2017-03-11T11:00:00")
        self.t("track 2017-03-13T10:00:00 - 2017-03-13T11:00:00")

        code, out, err = self.t("summary 2017-03-11 - 2017-03-14 :ids")

        self.assertIn("""
Wk  Date       Day ID Tags    Start      End    Time   Total
--- ---------- --- -- ---- -------- -------- ------- -------
W10 2017-03-11 Sat @2      10:00:00 11:00:00 1:00:00 1:00:00
W11 2017-03-13 Mon @1      10:00:00 11:00:00 1:00:00 1:00:00

                                                     2:00:00
""", out)

    def test_with_several_tracks_per_day(self):
        """Summary should display daily total"""
        self.t("track FOO 2017-03-09T10:00:00 - 2017-03-09T11:00:00")
        self.t("track BAR 2017-03-09T12:00:00 - 2017-03-09T13:00:00")
        self.t("track BAZ 2017-03-09T14:00:00 - 2017-03-09T15:00:00")
        self.t("track FOO 2017-03-11T10:00:00 - 2017-03-11T11:00:00")
        self.t("track BAR 2017-03-11T12:00:00 - 2017-03-11T13:00:00")
        self.t("track BAZ 2017-03-11T14:00:00 - 2017-03-11T15:00:00")

        code, out, err = self.t("summary 2017-03-09 - 2017-03-12 :ids")

        self.assertIn("""
Wk  Date       Day ID Tags    Start      End    Time   Total
--- ---------- --- -- ---- -------- -------- ------- -------
W10 2017-03-09 Thu @6 FOO  10:00:00 11:00:00 1:00:00
                   @5 BAR  12:00:00 13:00:00 1:00:00
                   @4 BAZ  14:00:00 15:00:00 1:00:00 3:00:00
W10 2017-03-11 Sat @3 FOO  10:00:00 11:00:00 1:00:00
                   @2 BAR  12:00:00 13:00:00 1:00:00
                   @1 BAZ  14:00:00 15:00:00 1:00:00 3:00:00

                                                     6:00:00
""", out)

    def test_with_empty_interval_at_start_of_day(self):
        """Summary should display empty intervals at midnight"""
        self.t("track sod - sod")
        code, out, err = self.t("summary :year")
        self.assertRegexpMatches(out, """
Wk  ?Date       Day Tags    ?Start      ?End    Time   Total
[ -]+
W\d{1,2} \d{4}-\d{2}-\d{2} .{3}       ?0:00:00 0:00:00 0:00:00 0:00:00

[ ]+0:00:00
""")


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
