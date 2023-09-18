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

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestIds(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_ids_in_output(self):
        self.t("start a 10s ago")
        self.t("start b")
        code, out, err = self.t("summary :week :ids")
        self.assertIn(' @1 ', out)
        self.assertIn(' @2 ', out)

    def test_latest_interval_included_when_empty(self):
      """Count IDs when the last interval is empty
      Include the last interval in getTracked() even if it is a
      zero-width interval and there are other, earlier intervals.
      """
      self.t("track 2018-01-01 - 2018-01-01")
      self.t("track 2018-01-02 - 2018-01-02")
      code, out, err = self.t("move @2 2018-01-03")
      self.assertIn('Moved @2 to 2018-01-03T00:00:00', out)

    def test_should_fail_on_zero_id(self):
        code, out, err = self.t.runError("delete @0")
        self.assertIn("'@0' is not a valid ID.", err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
