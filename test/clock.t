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

from time import sleep

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestClock(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_stop_nothing(self):
        """Verify that stopping an unstarted interval does nothing"""
        code, out, err = self.t.runError("stop")
        self.assertIn("There is no active time tracking.", err)

    def test_start_new(self):
        """Verify that 'start' creates an open interval"""
        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

        code, out, err = self.t("start tag1 tag2")
        self.assertIn("Tracking tag1 tag2\n", out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["tag1", "tag2"])

    def test_start_stop(self):
        """Verify that start/stop creates and closes an interval"""
        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

        code, out, err = self.t("start tag1 tag2")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t("stop")
        self.assertIn("Recorded tag1 tag2\n", out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["tag1", "tag2"])

        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

    def test_start_additional(self):
        """Verify that 'start' closes an open interval and starts a new one"""
        code, out, err = self.t.runError("")
        self.assertIn("There is no active time tracking.", out)

        code, out, err = self.t("start tag1 tag2 2h ago")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t("start tag3 1h ago")
        self.assertIn("Recorded tag1 tag2\n", out)
        self.assertIn("Tracking tag3\n", out)

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0], expectedTags=["tag1", "tag2"])
        self.assertOpenInterval(j[1], expectedTags=["tag3"])

    def test_start_subtract(self):
        """Verify that starting multiple tags and stopping one leaves an open interval"""
        code, out, err = self.t("start tag1 tag2 tag3 2h ago")
        self.assertIn("Tracking tag1 tag2 tag3\n", out)

        code, out, err = self.t("stop tag1 1h ago")
        self.assertIn("Recorded tag1 tag2 tag3\n", out)
        self.assertIn("Tracking tag2 tag3\n", out)

        code, out, err = self.t("stop")
        self.assertIn("Recorded tag2 tag3\n", out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
