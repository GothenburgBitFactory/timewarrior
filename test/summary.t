#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2006 - 2017, Paul Beckingham, Federico Hernandez.
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
# http://www.opensource.org/licenses/mit-license.php
#
###############################################################################

import sys
import os
import unittest
from datetime import datetime
# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase

# Test methods available:
#     self.assertEqual(a, b)
#     self.assertNotEqual(a, b)
#     self.assertTrue(x)
#     self.assertFalse(x)
#     self.assertIs(a, b)
#     self.assertIsNot(substring, text)
#     self.assertIsNone(x)
#     self.assertIsNotNone(x)
#     self.assertIn(substring, text)
#     self.assertNotIn(substring, text
#     self.assertRaises(e)
#     self.assertRegexpMatches(text, pattern)
#     self.assertNotRegexpMatches(text, pattern)
#     self.tap("")

class TestSummary(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_empty(self):
        code, out, err = self.t("summary")
        self.assertIn("No filtered data found in the range", out)

    def test_filled(self):
        self.t("start 1h ago")
        self.t("stop")

        code, out, err = self.t("summary :ids")

        self.assertRegexpMatches(out, """
Wk  Date       Day ID Tags    ?Start      ?End    Time   Total
[ -]+
W[0-9]{2} [0-9]{4}-[0-9]{2}-[0-9]{2} .{3} @1      [0-9]{1,2}:[0-9]{2}:[0-9]{2} [0-9]{1,2}:[0-9]{2}:[0-9]{2} 1:00:00 1:00:00

[ ]+1:00:00
""")

    def test_with_range_filter(self):
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

    def test_with_tag_filter(self):


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

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
