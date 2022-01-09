#!/usr/bin/env python3

###############################################################################
#
# Copyright 2017 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

import sys

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestResize(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_resize_closed_interval(self):
        """Resize a closed interval"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")
        code, out, err = self.t("resize @1 10mins")
        self.assertIn('Resized @1 to 0:10:00', out)

    def test_resize_open_interval(self):
        """Resize an open interval"""
        self.t("start 30mins ago foo")
        code, out, err = self.t.runError("resize @1 10mins")
        self.assertIn('Cannot resize open interval @1', err)

    def test_resize_full_month_interval(self):
        """Resize an interval to cover a full month"""
        self.t("track now - now")
        self.t("resize @1 1month")
        code, out, err = self.t("resize @1 1month")
        self.assertIn('Resized @1 to 720:00:00', out)

    def test_resize_interval_to_enclose_month_border(self):
        """Resize an interval to enclose a month border"""
        self.t("track 20180831T220000 - 20180831T230000 foo")
        self.t("resize @1 3h")

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0])

    def test_referencing_a_non_existent_interval_is_an_error(self):
        """Calling resize with a non-existent interval reference is an error"""
        code, out, err = self.t.runError("resize @1 @2 10min")
        self.assertIn("ID '@1' does not correspond to any tracking.", err)

        self.t("start 1h ago bar")

        code, out, err = self.t.runError("resize @2 10min")
        self.assertIn("ID '@2' does not correspond to any tracking.", err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
