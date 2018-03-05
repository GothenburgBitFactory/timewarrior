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
# http://www.opensource.org/licenses/mit-license.php
#
###############################################################################

import sys
import os
import unittest
from datetime import datetime, timedelta

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

class TestContinue(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_continue_nothing(self):
        """Verify that continuing an empty db is an error"""
        code, out, err = self.t.runError("continue")
        self.assertIn("There is no previous tracking to continue.", err)

    def test_continue_open(self):
        """Verify that continuing an open interval is an error"""
        code, out, err = self.t("start tag1 tag2 1h ago")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t.runError("continue")
        self.assertIn("There is already active tracking.", err)

    def test_continue_closed(self):
        """Verify that continuing a closed interval works"""
        code, out, err = self.t("start tag1 tag2 1h ago")
        self.assertIn("Tracking tag1 tag2\n", out)

        code, out, err = self.t("stop")
        self.assertIn("Recorded tag1 tag2\n", out)

        code, out, err = self.t("continue")
        self.assertIn("Tracking tag1 tag2\n", out)

    def test_continue_with_multiple_ids(self):
        """Verify that 'continue' with multiple ids is an error"""
        code, out, err = self.t.runError("continue @1 @2")
        self.assertIn("You can only specify one ID to continue.\n", err)

    def test_continue_with_invalid_id(self):
        """Verify that 'continue' with invalid id is an error"""
        code, out, err = self.t("start FOO 1h ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop 30min ago")
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t.runError("continue @4")
        self.assertIn("ID '@4' does not correspond to any tracking.\n", err)

    def test_continue_with_id_without_active_tracking(self):
        """Verify that continuing a specified interval works"""
        code, out, err = self.t("start FOO 1h ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("start BAR 30min ago")
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop 15min ago")
        self.assertIn("Recorded BAR\n", out)

        code, out, err = self.t("continue @2")
        self.assertIn("Tracking FOO\n", out)

    def test_continue_with_id_with_active_tracking(self):
        """Verify that continuing a specified interval stops active tracking"""
        code, out, err = self.t("start FOO 1h ago")
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("start BAR 30min ago")
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("continue @2")
        self.assertIn("Recorded BAR\n", out)
        self.assertIn("Tracking FOO\n", out)

    def test_continue_with_id_and_date(self):
        """Verify that continuing a specified interval with date continues at given date"""
        now = datetime.now()
        now_utc = now.utcnow()

        two_hours_before = now - timedelta(hours=2)
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        code, out, err = self.t("start FOO {:%Y-%m-%dT%H}:00:00".format(five_hours_before))
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00".format(four_hours_before))
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t("start BAR {:%Y-%m-%dT%H}:00:00".format(four_hours_before))
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00".format(three_hours_before))
        self.assertIn("Recorded BAR\n", out)

        self.t("continue @2 {:%Y-%m-%dT%H}:00:00".format(two_hours_before))

        j = self.t.export()

        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.assertEqual(len(j), 3)

        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}0000Z'.format(five_hours_before_utc), 'start time of first interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(five_hours_before_utc, j[0]['start']))
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}0000Z'.format(four_hours_before_utc), 'end time of first interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(four_hours_before_utc, j[0]['end']))
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'], ['FOO'])

        self.assertTrue('start' in j[1])
        self.assertEqual(j[1]['start'], '{:%Y%m%dT%H}0000Z'.format(four_hours_before_utc), 'start time of second interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(four_hours_before_utc, j[1]['start']))
        self.assertTrue('end' in j[1])
        self.assertEqual(j[1]['end'], '{:%Y%m%dT%H}0000Z'.format(three_hours_before_utc), 'end time of second interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(three_hours_before_utc, j[1]['end']))
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'], ['BAR'])

        self.assertTrue('start' in j[2])
        self.assertEqual(j[2]['start'], '{:%Y%m%dT%H}0000Z'.format(two_hours_before_utc), 'start time of continued interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(two_hours_before_utc, j[2]['start']))
        self.assertFalse('end' in j[2])
        self.assertTrue('tags' in j[2])
        self.assertEqual(j[2]['tags'], ['FOO'])

    def test_continue_with_id_and_range(self):
        """Verify that continue with a range adds a copy with same tags"""
        now = datetime.now()
        now_utc = now.utcnow()

        one_hour_before = now - timedelta(hours=1)
        two_hours_before = now - timedelta(hours=2)
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        code, out, err = self.t("start FOO {:%Y-%m-%dT%H}:00:00".format(five_hours_before))
        self.assertIn("Tracking FOO\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00".format(four_hours_before))
        self.assertIn("Recorded FOO\n", out)

        code, out, err = self.t("start BAR {:%Y-%m-%dT%H}:00:00".format(four_hours_before))
        self.assertIn("Tracking BAR\n", out)

        code, out, err = self.t("stop {:%Y-%m-%dT%H}:00:00".format(three_hours_before))
        self.assertIn("Recorded BAR\n", out)

        self.t("continue @2 {:%Y-%m-%dT%H}:00:00 - {:%Y-%m-%dT%H}:00:00".format(two_hours_before, one_hour_before))

        j = self.t.export()

        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.assertEqual(len(j), 3)

        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}0000Z'.format(five_hours_before_utc), 'start time of first interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(five_hours_before_utc, j[0]['start']))
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}0000Z'.format(four_hours_before_utc), 'end time of first interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(four_hours_before_utc, j[0]['end']))
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'], ['FOO'])

        self.assertTrue('start' in j[1])
        self.assertEqual(j[1]['start'], '{:%Y%m%dT%H}0000Z'.format(four_hours_before_utc), 'start time of second interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(four_hours_before_utc, j[1]['start']))
        self.assertTrue('end' in j[1])
        self.assertEqual(j[1]['end'], '{:%Y%m%dT%H}0000Z'.format(three_hours_before_utc), 'end time of second interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(three_hours_before_utc, j[1]['end']))
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'], ['BAR'])

        self.assertTrue('start' in j[2])
        self.assertEqual(j[2]['start'], '{:%Y%m%dT%H}0000Z'.format(two_hours_before_utc), 'start time of added interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(two_hours_before_utc, j[2]['start']))
        self.assertTrue('end' in j[2])
        self.assertEqual(j[2]['end'], '{:%Y%m%dT%H}0000Z'.format(one_hour_before_utc), 'end time of added interval does not match: expected {:%Y%m%dT%H}0000Z, actual {}'.format(one_hour_before_utc, j[2]['end']))
        self.assertTrue('tags' in j[2])
        self.assertEqual(j[2]['tags'], ['FOO'])

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
