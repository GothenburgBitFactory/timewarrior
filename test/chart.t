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

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestChart(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_chart_day_with_invalid_config_for_lines(self):
        """Chart should report error on invalid value for 'reports.day.lines'"""
        code, out, err = self.t.runError("day rc.reports.day.lines=foobar")

        self.assertIn("Invalid integer value for 'reports.day.lines': 'foobar'", err)

    def test_chart_day_with_invalid_config_for_cell(self):
        """Chart should report error on invalid value for 'reports.day.cell'"""
        code, out, err = self.t.runError("day rc.reports.day.cell=foobar")

        self.assertIn("Invalid integer value for 'reports.day.cell': 'foobar'", err)

    def test_chart_week_with_invalid_config_for_lines(self):
        """Chart should report error on invalid value for 'reports.week.lines'"""
        code, out, err = self.t.runError("week rc.reports.week.lines=foobar")

        self.assertIn("Invalid integer value for 'reports.week.lines': 'foobar'", err)

    def test_chart_week_with_invalid_config_for_cell(self):
        """Chart should report error on invalid value for 'reports.week.cell'"""
        code, out, err = self.t.runError("week rc.reports.week.cell=foobar")

        self.assertIn("Invalid integer value for 'reports.week.cell': 'foobar'", err)

    def test_chart_month_with_invalid_config_for_lines(self):
        """Chart should report error on invalid value for 'reports.month.lines'"""
        code, out, err = self.t.runError("month rc.reports.month.lines=foobar")

        self.assertIn("Invalid integer value for 'reports.month.lines': 'foobar'", err)

    def test_chart_month_with_invalid_config_for_cell(self):
        """Chart should report error on invalid value for 'reports.month.cell'"""
        code, out, err = self.t.runError("month rc.reports.month.cell=foobar")

        self.assertIn("Invalid integer value for 'reports.month.cell': 'foobar'", err)

    def test_chart_day_with_less_than_one_minute_interval_at_day_start(self):
        self.t("track 2016-01-15T00:00:00 - 2016-01-15T00:00:40 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO")
        code, out, err = self.t("day 2016-01-15 - 2016-01-16")

        self.assertIn("""\
\nFri 15 XOXO 1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23  \
\n       XOXO                                                                                                                   \
\n
       Tracked         0:00:40
       Available      23:59:20
       Total          24:00:00

""", out)

    def test_chart_day_with_less_than_one_minute_interval(self):
        self.t(
            "track 2016-01-15T02:00:00 - 2016-01-15T02:00:40 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO")
        code, out, err = self.t("day 2016-01-15 - 2016-01-16")

        self.assertIn("""\
\nFri 15 0    1    XOXO 3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23  \
\n                 XOXO                                                                                                         \
\n
       Tracked         0:00:40
       Available      23:59:20
       Total          24:00:00

""", out)

    def test_chart_day_with_less_than_one_hour_interval_at_day_start(self):
        self.t(
            "track 2016-01-15T00:00:00 - 2016-01-15T00:30:00 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO")
        code, out, err = self.t("day 2016-01-15 - 2016-01-16")

        self.assertIn("""\
\nFri 15 XO   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23  \
\n       XO                                                                                                                     \
\n
       Tracked         0:30:00
       Available      23:30:00
       Total          24:00:00

""", out)

    def test_chart_day_with_less_than_one_hour_interval(self):
        self.t(
            "track 2016-01-15T02:00:00 - 2016-01-15T02:30:00 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO")
        code, out, err = self.t("day 2016-01-15 - 2016-01-16")

        self.assertIn("""\
\nFri 15 0    1    XO   3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23  \
\n                 XO                                                                                                           \
\n
       Tracked         0:30:00
       Available      23:30:00
       Total          24:00:00

""", out)

    def test_chart_day_with_interval_over_day_border(self):
        self.t(
            "track 2016-01-15T23:00:00 - 2016-01-16T01:00:00 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO")

        code, out, err = self.t("day 2016-01-15 - 2016-01-17")
        self.assertIn("""\
\nFri 15 0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   XOXOX\
\n                                                                                                                          OXOXO\
\nSat 16 XOXOX1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23  \
\n       OXOXO                                                                                                                  \
\n
       Tracked         2:00:00
       Available      46:00:00
       Total          48:00:00

""", out)

    def test_chart_day_with_interval_over_day_border(self):
        self.t("track 2016-01-15T23:00:00 - 2016-01-16T01:00:00 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO")
        code, out, err = self.t("day 2016-01-15 - 2016-01-17")

        self.assertIn("""\
\nFri 15 0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   XOXOX\
\n                                                                                                                          OXOXO\
\nSat 16 XOXOX1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23  \
\n       OXOXO                                                                                                                  \
\n
       Tracked         2:00:00
       Available      46:00:00
       Total          48:00:00

""", out)

    def test_chart_day_with_interval_over_whole_day(self):
        self.t("track 2016-01-15T00:00:00 - 2016-01-16T00:00:00 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO")
        code, out, err = self.t("day 2016-01-15 - 2016-01-16")

        self.assertIn("""\
\nFri 15 XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO\
\n       XOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXOXO\
\n
       Tracked        24:00:00
       Available       0:00:00
       Total          24:00:00

""", out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
