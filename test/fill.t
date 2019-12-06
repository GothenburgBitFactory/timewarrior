#!/usr/bin/env python3

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

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestFillHint(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_unfilled_track_in_gap(self):
        """Add closed interval into a gap without fill"""
        self.t("track 20160709T050000Z - 20160709T060000Z one")
        self.t("track 20160709T090000Z - 20160709T100000Z three")

        code, out, err = self.t("track 20160709T070000Z - 20160709T080000Z two")

        self.assertNotIn('Backfilled to ', out)
        self.assertNotIn('Filled to ', out)

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="20160709T050000Z",
                                  expectedEnd="20160709T060000Z",
                                  expectedTags=["one"])
        self.assertClosedInterval(j[1],
                                  expectedStart="20160709T070000Z",
                                  expectedEnd="20160709T080000Z",
                                  expectedTags=["two"])
        self.assertClosedInterval(j[2],
                                  expectedStart="20160709T090000Z",
                                  expectedEnd="20160709T100000Z",
                                  expectedTags=["three"])

    def test_filled_track_in_gap(self):
        """Add closed interval into a gap with fill"""
        self.t("track 20160709T050000Z - 20160709T060000Z one")
        self.t("track 20160709T090000Z - 20160709T100000Z three")

        code, out, err = self.t("track 20160709T070000Z - 20160709T080000Z two :fill")

        self.assertIn('Backfilled to ', out)
        self.assertIn('Filled to ', out)

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart="20160709T050000Z",
                                  expectedEnd="20160709T060000Z",
                                  expectedTags=["one"])
        self.assertClosedInterval(j[1],
                                  expectedStart="20160709T060000Z",
                                  expectedEnd="20160709T090000Z",
                                  expectedTags=["two"])
        self.assertClosedInterval(j[2],
                                  expectedStart="20160709T090000Z",
                                  expectedEnd="20160709T100000Z",
                                  expectedTags=["three"])

    def test_filled_start(self):
        """Add an open interval with fill"""
        self.t("track 20160710T100000Z - 20160710T110000Z one")

        code, out, err = self.t("start 20160710T113000Z two :fill")

        self.assertIn('Backfilled to ', out)
        self.assertNotIn('Filled to ', out)

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
                                  expectedStart="20160710T100000Z",
                                  expectedEnd="20160710T110000Z",
                                  expectedTags=["one"])
        self.assertOpenInterval(j[1],
                                expectedStart="20160710T110000Z",
                                expectedTags=["two"])


class TestFillCommand(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

#    def test_fill_command(self):
#        """Create gaps, then fill them"""
#        self.t("track 20160724T090000 - 20160724T100000 foo")
#        # 11:12 gap here.
#        self.t("track 20160724T101112 - 20160724T101213 bar")
#        # 47:47 gap here.
#        self.t("track 20160724T110000 - 20160724T120000 baz")
#
#        # Eliminate gaps.
#        code, out, err = self.t("fill @2")
#        self.assertIn('Backfilled @2 to 2016-07-24T10:00:00', out)
#        self.assertIn('Filled @2 to 2016-07-24T11:00:00', out)

    def test_fill_last_interval(self):
        """TI-75: The :fill hint not properly detecting the last interval"""
        self.t("track 20170805T0100 - 20170805T0200 tag1")
        self.t("track 20170805T0200 - 20170805T0300 tag2")

        # Gap 0300 - 0400

        self.t("start 20170805T0400 tag3")

        code, out, err = self.t("summary :ids")

        self.tap(out)
        self.tap(err)

        code, out, err = self.t("track :fill 20170805T0300 - 20170805T0330 tag4")

        self.tap(out)
        self.tap(err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
