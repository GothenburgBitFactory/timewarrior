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


class TestDelete(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_delete_open(self):
        """Delete a single open interval"""
        self.t("track :yesterday foo")
        self.t("start today bar")

        code, out, err = self.t("delete @1")
        self.assertEqual(out, 'Deleted @1\n')

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_delete_closed(self):
        """Delete a single closed interval"""
        self.t("track :yesterday foo")
        self.t("start today bar")
        code, out, err = self.t("delete @2")
        self.assertEqual(out, 'Deleted @2\n')

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertFalse('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'bar')

    def test_delete_multiple(self):
        """Delete a mix of open/closed intervals"""
        self.t("track :yesterday foo")
        self.t("start today bar")
        code, out, err = self.t("delete @1 @2")
        self.assertIn('Deleted @1', out)
        self.assertIn('Deleted @2', out)

        j = self.t.export()
        self.assertEqual(len(j), 0)

    def test_delete_open_interval_spanning_exclusion(self):
        """Delete an open interval that spans over an exclusion"""
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

        self.t("start {:%Y-%m-%dT%H}:00:00 foo".format(five_hours_before))

        # Delete the open interval.
        code, out, err = self.t("delete @1")  # self.t("delete @1 :debug")
        print(out)
        self.assertEqual(out, 'Deleted @1\n')

        # The last interval should be closed, because the open interval was deleted.
        j = self.t.export()
        print(j)
        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                  expectedStart='{:%Y%m%dT%H}0000Z'.format(now_utc-timedelta(hours=5)),
                                  expectedEnd='{:%Y%m%dT%H}0000Z'.format(now_utc-timedelta(hours=4)),
                                  expectedTags=['foo'],
                                  description='remaining interval')


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
