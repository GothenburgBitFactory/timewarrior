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


class TestLengthen(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_lengthen_closed_interval(self):
        """Lengthen a closed interval"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")
        code, out, err = self.t("lengthen @1 10mins")
        self.assertIn('Lengthened @1 by 0:10:00', out)

    def test_lengthen_open_interval(self):
        """Lengthen an open interval"""
        self.t("start 30mins ago foo")
        code, out, err = self.t.runError("lengthen @1 10mins")
        self.assertIn('Cannot lengthen open interval @1', err)

    def test_lengthen_synthetic_interval(self):
        """Lengthen a synthetic interval."""
        now = datetime.now()
        now_utc = now.utcnow()

        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        if four_hours_before.day < three_hours_before.day:
            exclusion = "<{:%H}:00 >{:%H}:00".format(three_hours_before, four_hours_before)
        else:
            exclusion = "{:%H}:00-{:%H}:00".format(four_hours_before, three_hours_before)

        self.t.config("exclusions.sunday", exclusion)
        self.t.config("exclusions.monday", exclusion)
        self.t.config("exclusions.tuesday", exclusion)
        self.t.config("exclusions.wednesday", exclusion)
        self.t.config("exclusions.thursday", exclusion)
        self.t.config("exclusions.friday", exclusion)
        self.t.config("exclusions.saturday", exclusion)

        self.t("start {:%Y-%m-%dT%H}:45:00".format(five_hours_before))

        self.t("lengthen @2 5min")

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}4500Z'.format(now_utc-timedelta(hours=5)), 'start time of lengthened interval does not match')
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}0500Z'.format(now_utc - timedelta(hours=4)), 'end time of lengthened interval does not match')
        self.assertFalse('tags' in j[0])
        self.assertTrue('start' in j[1])
        self.assertEqual(j[1]['start'], '{:%Y%m%dT%H}0000Z'.format(now_utc - timedelta(hours=3)), 'start time of unmodified interval does not match')
        self.assertFalse('end' in j[1])
        self.assertFalse('tags' in j[1])


# TODO Add :adjust tests.


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
