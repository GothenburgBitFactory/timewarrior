#!/usr/bin/env python
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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


class TestUndo(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_undo_annotate(self):
        """Test undo of command 'annotate'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("annotate @1 \"lorem ipsum\"")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedAnnotation="lorem ipsum")

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedAnnotation="")

    def test_undo_cancel(self):
        """Test undo of command 'cancel'"""
        one_hour_before_utc = datetime.now().utcnow() - timedelta(hours=1)

        self.t("start {:%Y%m%dT%H%M%SZ} foo".format(one_hour_before_utc))
        self.t("cancel")

        j = self.t.export()
        self.assertEqual(len(j), 0, msg="Expected 0 interval before, got {}".format(len(j)))

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertOpenInterval(j[0],
                                expectedStart=one_hour_before_utc,
                                expectedTags=["foo"])

    def test_undo_config_add_name(self):
        """Test undo of command 'config' (add name)"""
        self.t("config foo bar :yes")

        before = [x for x in self.t.timewrc_content if x != '\n']

        self.t("undo")

        after = [x for x in self.t.timewrc_content if x != '\n']

        self.assertNotEqual(before, after)
        self.assertEqual([], after)

    def test_undo_config_remove_name(self):
        """Test undo of command 'config' (remove name)"""
        self.t("config foo bar :yes")
        self.t("config foo :yes")

        before = self.t.timewrc_content[:]

        self.t("undo")

        after = self.t.timewrc_content[:]

        self.assertNotEqual(before, after)

    def test_undo_config_set_value(self):
        """Test undo of command 'config' (set value)"""
        self.t("config foo bar :yes")
        self.t("config foo baz :yes")

        before = self.t.timewrc_content[:]

        self.t("undo")

        after = self.t.timewrc_content[:]

        self.assertNotEqual(before, after)

    def test_undo_config_remove_value(self):
        """Test undo of command 'config' (remove value)"""
        self.t("config foo bar :yes")
        self.t("config foo '' :yes")

        before = self.t.timewrc_content[:]

        self.t("undo")

        after = self.t.timewrc_content[:]

        self.assertNotEqual(before, after)

    def test_undo_continue(self):
        """Test undo of command 'continue'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("continue @1 {:%Y%m%dT%H%M%SZ}".format(now_utc))

        j = self.t.export()
        self.assertEqual(len(j), 2, msg="Expected 2 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])
        self.assertOpenInterval(j[1],
                                expectedStart=now_utc,
                                expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

    def test_undo_delete(self):
        """Test undo of command 'delete'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("delete @1")

        j = self.t.export()
        self.assertEqual(len(j), 0, msg="Expected 0 intervals before, got {}".format(len(j)))

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

    # def test_undo_fill(self):
    # """Test undo of command 'fill 'Not yet implemented
    #   self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo bar")
    #   self.t("untag @1 bar")
    #
    #   j = self.t.export()
    #   self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
    #   self.assertClosedInterval(j[0],
    #                             expectedStart="20180101T060000",
    #                             expectedEnd="20180101T070000",
    #                             expectedTags=["foo"])
    #
    #   self.t("undo")
    #
    #   j = self.t.export()
    #   self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
    #   self.assertClosedInterval(j[0],
    #                             expectedStart="20180101T060000",
    #                             expectedEnd="20180101T070000",
    #                             expectedTags=["foo bar"])

    def test_undo_join(self):
        """Test undo of command 'join'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} bar".format(four_hours_before_utc, three_hours_before_utc))
        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("join @1 @2")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["bar", "foo"],
                                  description="joined interval")

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 2, msg="Expected 2 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before_utc,
                                  expectedEnd=three_hours_before_utc,
                                  expectedTags=["bar"],
                                  description="first interval")
        self.assertClosedInterval(j[1],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"],
                                  description="second interval")

    def test_undo_lengthen(self):
        """Test undo of command 'lengthen'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(three_hours_before_utc, two_hours_before_utc))
        self.t("lengthen @1 1h")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=two_hours_before_utc,
                                  expectedTags=["foo"])

    def test_undo_move(self):
        """Test undo of command 'move'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("move @1 {:%Y%m%dT%H%M%SZ}".format(three_hours_before_utc))

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=two_hours_before_utc,
                                  expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

    def test_undo_resize(self):
        """Test undo of command 'resize'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(three_hours_before_utc, one_hour_before_utc))
        self.t("resize @1 1h")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=two_hours_before_utc,
                                  expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

    def test_undo_shorten(self):
        """Test undo of command 'shorten'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(three_hours_before_utc, one_hour_before_utc))
        self.t("shorten @1 1h")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=two_hours_before_utc,
                                  expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

    def test_undo_split(self):
        """Test undo of command 'split'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(three_hours_before_utc, one_hour_before_utc))
        self.t("split @1")

        j = self.t.export()
        self.assertEqual(len(j), 2, msg="Expected 2 intervals before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=two_hours_before_utc,
                                  expectedTags=["foo"])
        self.assertClosedInterval(j[1],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=three_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

    def test_undo_start(self):
        """Test undo of command 'start'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y%m%dT%H%M%SZ} foo".format(one_hour_before_utc))

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertOpenInterval(j[0],
                                expectedStart=one_hour_before_utc,
                                expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 0, msg="Expected 0 interval afterwards, got {}".format(len(j)))

    def test_undo_consecutive_start(self):
        """Test undo of consecutive commands 'start'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("start {:%Y%m%dT%H%M%SZ} foo".format(two_hours_before_utc))
        self.t("start {:%Y%m%dT%H%M%SZ} bar".format(one_hour_before_utc))

        j = self.t.export()
        self.assertEqual(len(j), 2, msg="Expected 2 intervals before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])
        self.assertOpenInterval(j[1],
                                expectedStart=one_hour_before_utc,
                                expectedTags=["bar"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertOpenInterval(j[0],
                                expectedStart=two_hours_before_utc,
                                expectedTags=["foo"])

    def test_undo_stop(self):
        """Test undo of command 'stop'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y%m%dT%H%M%SZ} foo".format(one_hour_before_utc))
        self.t("stop {:%Y%m%dT%H%M%SZ}".format(now_utc))

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=one_hour_before_utc,
                                  expectedEnd=now_utc,
                                  expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertOpenInterval(j[0],
                                expectedStart=one_hour_before_utc,
                                expectedTags=["foo"])

    def test_undo_tag(self):
        """Test undo of command 'tag'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(two_hours_before_utc, one_hour_before_utc))
        self.t("tag @1 bar")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["bar", "foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

    def test_undo_track(self):
        """Test undo of command 'track'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(four_hours_before_utc, three_hours_before_utc))
        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} bar".format(two_hours_before_utc, one_hour_before_utc))

        j = self.t.export()
        self.assertEqual(len(j), 2, msg="Expected 2 intervals before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before_utc,
                                  expectedEnd=three_hours_before_utc,
                                  expectedTags=["foo"])
        self.assertClosedInterval(j[1],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["bar"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before_utc,
                                  expectedEnd=three_hours_before_utc,
                                  expectedTags=["foo"])

    def test_undo_track_with_adjust_hint(self):
        """Test undo of command 'track' with adjust hint"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        six_hours_before_utc = now_utc - timedelta(hours=6)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo".format(four_hours_before_utc, three_hours_before_utc))
        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} bar".format(two_hours_before_utc, one_hour_before_utc))

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} xyz :adjust".format(six_hours_before_utc, now_utc))

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 2 intervals before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=six_hours_before_utc,
                                  expectedEnd=now_utc,
                                  expectedTags=["xyz"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 2, msg="Expected 2 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=four_hours_before_utc,
                                  expectedEnd=three_hours_before_utc,
                                  expectedTags=["foo"])
        self.assertClosedInterval(j[1],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["bar"])

    def test_undo_untag(self):
        """Test undo of command 'untag'"""
        now_utc = datetime.now().utcnow()
        one_hour_before_utc = now_utc - timedelta(hours=1)
        two_hours_before_utc = now_utc - timedelta(hours=2)

        self.t("track {:%Y%m%dT%H%M%SZ} - {:%Y%m%dT%H%M%SZ} foo bar".format(two_hours_before_utc, one_hour_before_utc))
        self.t("untag @1 bar")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval before, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["foo"])

        self.t("undo")

        j = self.t.export()
        self.assertEqual(len(j), 1, msg="Expected 1 interval afterwards, got {}".format(len(j)))
        self.assertClosedInterval(j[0],
                                  expectedStart=two_hours_before_utc,
                                  expectedEnd=one_hour_before_utc,
                                  expectedTags=["bar", "foo"])


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
