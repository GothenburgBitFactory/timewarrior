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


class TestMove(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_move_closed_forwards(self):
        """Move a closed interval forwards in time"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")
        code, out, err = self.t("move @1 2016-01-01T02:00:00")
        self.assertIn('Moved @1 to 2016-01-01T02:00:00', out)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_move_closed_backwards(self):
        """Move a closed interval backwards in time"""
        self.t("track 2016-01-01T04:00:00 - 2016-01-01T05:00:00 foo")
        code, out, err = self.t("move @1 2016-01-01T03:00:00")
        self.assertIn('Moved @1 to 2016-01-01T03:00:00', out)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_move_open_backwards_to_specific_time(self):
        """Move an open interval backwards to specific time."""
        one_hour_before = datetime.now() - timedelta(hours=1)

        self.t("start 5mins ago foo")
        code, out, err = self.t("move @1 {:%Y-%m-%dT%H}:01:23".format(one_hour_before))
        self.assertRegexpMatches(out, "Moved @1 to {:%Y-%m-%dT%H}:01:23".format(one_hour_before))

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' not in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_move_open_backwards(self):
        """Move an open interval backwards in time"""
        self.t("start 5mins ago foo")
        code, out, err = self.t("move @1 today")
        self.assertRegexpMatches(out, 'Moved @1 to \d\d\d\d-\d\d-\d\dT00:00:00')

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' not in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_move_open_forwards(self):
        """Move an open interval forwards in time"""
        self.t("start yesterday foo")
        code, out, err = self.t("move @1 today")
        self.assertRegexpMatches(out, 'Moved @1 to \d\d\d\d-\d\d-\d\dT00:00:00')

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' not in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

    def test_move_interval_over_another_with_adjust(self):
        """Move an interval over another with :adjust"""

        code, out, err = self.t("track 20170301T110000Z - 20170301T140000Z foo")
        code, out, err = self.t("track 20170301T150000Z - 20170301T160000Z foo")

        # Move the interval.
        code, out, err = self.t("move @1 20170301T133000Z :adjust")
        # Display is in local time zone so we can't match time exactly.
        self.assertRegexpMatches(out, 'Moved @1 to 2017-03-01T\d\d:\d\d:\d\d\n')

        # There should now be an interval starting
        # at 13:30.
        starts = [str(x['start']) for x in self.t.export()]
        self.assertIn('20170301T133000Z', starts)

        # There should no longer be an interval ending at
        # 14:00 as that should have been adjusted.
        ends = [str(x['end']) for x in self.t.export()]
        self.assertNotIn('20170301T140000Z', ends)

    def test_move_synthetic_interval_into_exclusion(self):
        """Move a synthetic interval into exclusion"""
        now = datetime.now()
        now_utc = now.utcnow()

        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        if four_hours_before.day < three_hours_before.day:
            exclusion = "<{:%H}:00 >{:%H}:00".format(three_hours_before, four_hours_before)
        else:
            exclusion = "{:%H}:00-{:%H}:00".format(four_hours_before, three_hours_before)

        self.t.config("exclusions.friday", exclusion)
        self.t.config("exclusions.thursday", exclusion)
        self.t.config("exclusions.wednesday", exclusion)
        self.t.config("exclusions.tuesday", exclusion)
        self.t.config("exclusions.monday", exclusion)
        self.t.config("exclusions.sunday", exclusion)
        self.t.config("exclusions.saturday", exclusion)

        self.t("start {:%Y-%m-%dT%H}:45:00".format(five_hours_before))

        self.t("move @2 {:%Y-%m-%dT%H}:50:00".format(five_hours_before))

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}5000Z'.format(now_utc-timedelta(hours=5)), 'start time of lengthened interval does not match')
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}0500Z'.format(now_utc - timedelta(hours=4)), 'end time of lengthened interval does not match')
        self.assertFalse('tags' in j[0])
        self.assertTrue('start' in j[1])
        self.assertEqual(j[1]['start'], '{:%Y%m%dT%H}0000Z'.format(now_utc - timedelta(hours=3)), 'start time of unmodified interval does not match')
        self.assertFalse('end' in j[1])
        self.assertFalse('tags' in j[1])

    def test_move_synthetic_interval_away_from_exclusion(self):
        """Move a synthetic interval away from exclusion"""
        now = datetime.now()
        now_utc = now.utcnow()

        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)
        five_hours_before = now - timedelta(hours=5)

        if four_hours_before.day < three_hours_before.day:
            exclusion = "<{:%H}:00 >{:%H}:00".format(three_hours_before, four_hours_before)
        else:
            exclusion = "{:%H}:00-{:%H}:00".format(four_hours_before, three_hours_before)

        self.t.config("exclusions.friday", exclusion)
        self.t.config("exclusions.thursday", exclusion)
        self.t.config("exclusions.wednesday", exclusion)
        self.t.config("exclusions.tuesday", exclusion)
        self.t.config("exclusions.monday", exclusion)
        self.t.config("exclusions.sunday", exclusion)
        self.t.config("exclusions.saturday", exclusion)

        self.t("start {:%Y-%m-%dT%H}:45:00".format(five_hours_before))

        self.t("move @2 {:%Y-%m-%dT%H}:40:00".format(five_hours_before))

        j = self.t.export()

        self.assertEqual(len(j), 2)
        self.assertTrue('start' in j[0])
        self.assertEqual(j[0]['start'], '{:%Y%m%dT%H}4000Z'.format(now_utc-timedelta(hours=5)), 'start time of lengthened interval does not match')
        self.assertTrue('end' in j[0])
        self.assertEqual(j[0]['end'], '{:%Y%m%dT%H}5500Z'.format(now_utc - timedelta(hours=5)), 'end time of lengthened interval does not match')
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
