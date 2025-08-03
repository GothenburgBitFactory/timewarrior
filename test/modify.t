#!/usr/bin/env python3

###############################################################################
#
# Copyright 2018 - 2022, 2024, Gothenburg Bit Factory.
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
from datetime import datetime, timezone, timedelta
from dateutil import tz

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase

class TestModify(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_modify_end_of_open_interval(self):
        """Attempt to modify end of an open interval"""
        now_utc = datetime.now(timezone.utc)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))
        code, out, err = self.t.runError("modify end @1 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))
        self.assertIn("Cannot modify end of open interval", err)

    def test_modify_start_of_open_interval(self):
        """Modify start of open interval"""
        now_utc = datetime.now(timezone.utc)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))
        code, out, err = self.t("modify start @1 {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0],
            expectedStart="{:%Y%m%dT%H%M%S}Z".format(one_hour_before_utc))

    def test_modify_invalid_subcommand(self):
        """Modify with invalid subcommand"""
        now_utc = datetime.now(timezone.utc)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))
        self.t("stop")
        code, out, err = self.t.runError("modify @1 bogus {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc))
        self.assertIn("Must specify start|end|range command to modify", err)

    def test_modify_no_end_time(self):
        """Modify without a time to stop at"""
        now_utc = datetime.now(timezone.utc)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(one_hour_before_utc))
        self.t("stop")
        code, out, err = self.t.runError("modify end @1")
        self.assertIn("No updated time", err)

    def test_modify_shorten_one_hour(self):
        """Shorten the interval by one hour."""
        now_utc = datetime.now(timezone.utc)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t("modify end @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=2)))

        j = self.t.export()
        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
            expectedStart="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=3)),
            expectedEnd="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=2)))

    def test_modify_shorten_before_start(self):
        """Modify should not move end before start."""
        now_utc = datetime.now(timezone.utc)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify end @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=4)))
        self.assertIn("Cannot modify interval", err);

    def test_modify_start_to_after_end(self):
        """Modify should not move start beyond end."""
        now_utc = datetime.now(timezone.utc)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify start @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=59)))
        self.assertIn("Cannot modify interval", err);

    def test_modify_start_within_interval(self):
        """Increase start time within interval."""
        now_utc = datetime.now(timezone.utc)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t("modify start @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=2)))

        j = self.t.export()
        self.assertEqual(len(j), 2)
        self.assertClosedInterval(j[0],
            expectedStart="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=2)),
            expectedEnd="{:%Y%m%dT%H%M%S}Z".format(now_utc - timedelta(hours=1)))

    def test_modify_move_stop_to_overlap_following_interval(self):
        """Move end time to overlap with following interval."""
        now_utc = datetime.now(timezone.utc)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify end @2 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=30)))
        self.assertIn("You cannot overlap intervals", err)

    def test_modify_move_start_to_overlap_preceeding_interval(self):
        """Move start time to overlap with preceeding interval."""
        now_utc = datetime.now(timezone.utc)

        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=3)))
        self.t("stop {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=1)))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(minutes=49)))
        self.t("stop")

        code, out, err = self.t.runError("modify start @1 {:%Y-%m-%dT%H:%M:%S}Z".format(now_utc - timedelta(hours=2)))
        self.assertIn("You cannot overlap intervals", err)

    def test_modify_move_start_of_synthetic_interval_with_nonsynethic(self):
        """`timew modify` should be able to move start of synthetic interval in presence of non-synthetic intervals."""

        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        day_before = now_utc - timedelta(days=1)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        # Place a non-synthetic interval in the history before the exclusions
        self.t("track from {:%Y-%m-%dT%H:%M:%S}Z for 30min bar".format(day_before))
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        # Then modify the synthetic one
        self.t("modify start @1 {:%Y-%m-%dT%H:%M:%S}Z".format(three_hours_before_utc + timedelta(minutes=10)))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart=day_before,
                                  expectedEnd=day_before + timedelta(minutes=30),
                                  expectedTags=[],
                                  description="non-synthetic interval")
        self.assertClosedInterval(j[1],
                                  expectedStart=five_hours_before_utc,
                                  expectedEnd=four_hours_before_utc,
                                  expectedTags=[],
                                  description="unmodified interval")
        self.assertOpenInterval(j[2],
                                expectedStart=three_hours_before_utc + timedelta(minutes=10),
                                expectedTags=[],
                                description="moved interval")

    def test_modify_move_start_of_non_synthetic_interval_with_synethic(self):
        """`timew modify` should be able to move start of non-synthetic interval in presence of synthetic intervals."""

        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        day_before = now_utc - timedelta(days=1)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)
        five_hours_before_utc = now_utc - timedelta(hours=5)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        # Place a non-synthetic interval in the history before the exclusions
        self.t("track from {:%Y-%m-%dT%H:%M:%S}Z for 30min bar".format(day_before))

        # Open an interval that crosses the exclusions to create two synthetic
        # intervals
        self.t("start {:%Y-%m-%dT%H:%M:%S}Z foo".format(five_hours_before_utc))

        # Then modify the non-synthetic one
        self.t("modify start @3 {:%Y-%m-%dT%H:%M:%S}Z".format(day_before + timedelta(minutes=10)))

        j = self.t.export()

        self.assertEqual(len(j), 3)
        self.assertClosedInterval(j[0],
                                  expectedStart=day_before + timedelta(minutes=10),
                                  expectedEnd=day_before + timedelta(minutes=30),
                                  expectedTags=[],
                                  description="non-synthetic interval")
        self.assertClosedInterval(j[1],
                                  expectedStart=five_hours_before_utc,
                                  expectedEnd=four_hours_before_utc,
                                  expectedTags=[],
                                  description="unmodified interval")
        self.assertOpenInterval(j[2],
                                expectedStart=three_hours_before_utc,
                                expectedTags=[],
                                description="moved interval")

    def test_modify_move_start_inside_exclusion(self):
        """`timew modify` should handle moving start times within an exclusion."""

        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        three_hours_before = now - timedelta(hours=3)
        four_hours_before = now - timedelta(hours=4)

        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        four_hours_before_utc = now_utc - timedelta(hours=4)

        self.t.configure_exclusions((four_hours_before.time(), three_hours_before.time()))

        # Start an interval within the exclusion
        self.t("start {:%Y-%m-%dT%H:%M:%S} foo".format(four_hours_before + timedelta(minutes=20)))

        # Now modify the start time, but keep the start within the exclusion
        self.t("modify start @1 {:%Y-%m-%dT%H:%M:%S} :debug".format(four_hours_before + timedelta(minutes=10)))

        j = self.t.export() 

        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0],
                                expectedStart=four_hours_before_utc + timedelta(minutes=10),
                                expectedTags=['foo'])

    def test_referencing_a_non_existent_interval_is_an_error(self):
        """Calling modify with a non-existent interval reference is an error"""
        self.t("start 1h ago bar")

        code, out, err = self.t.runError("modify start @2")
        self.assertIn("ID '@2' does not correspond to any tracking.", err)

    def test_modify_range(self):
        """Call modify with range subcommand"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)

        self.t("track from {:%Y-%m-%dT%H:%M:%S}Z for 30min bar".format(four_hours_before_utc))

        self.t("modify @1 range {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z".format(three_hours_before_utc, two_hours_before_utc))

        j = self.t.export()

        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0],
                                expectedStart=three_hours_before_utc,
                                expectedEnd=two_hours_before_utc,
                                expectedTags=['bar'])

    def test_modify_range_with_point_in_time(self):
        """Call modify range with a point in time is an error"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        four_hours_before_utc = now_utc - timedelta(hours=4)

        self.t("track from {:%Y-%m-%dT%H:%M:%S}Z for 30min bar".format(four_hours_before_utc))

        self.t("modify @1 range {:%Y-%m-%dT%H:%M:%S}Z".format(three_hours_before_utc))

    def test_modify_range_with_fill_hint(self):
        """Call modify range with :fill hint to expand interval to fill gaps"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        
        # Create three intervals with gaps between them:
        # @3: 4 hours ago for 30min (4:00 - 3:30)
        # @2: 3:00 - 2:30 (gap: 3:30 - 3:00) 
        # @1: 1:30 - 1:00 (gap: 2:30 - 1:30)
        one_hour_before_utc = now_utc - timedelta(hours=1)
        one_hour_thirty_before_utc = now_utc - timedelta(hours=1, minutes=30)
        two_hour_thirty_before_utc = now_utc - timedelta(hours=2, minutes=30)
        three_hours_before_utc = now_utc - timedelta(hours=3)
        three_hour_thirty_before_utc = now_utc - timedelta(hours=3, minutes=30)
        four_hours_before_utc = now_utc - timedelta(hours=4)

        # Create intervals @3, @2, @1 (in reverse chronological order)
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_three".format(four_hours_before_utc, three_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_two".format(three_hours_before_utc, two_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_one".format(one_hour_thirty_before_utc, one_hour_before_utc))

        # Modify @2 with :fill hint - should expand to fill gaps between @3 and @1
        code, out, err = self.t("modify @2 range :fill")

        # Should see backfill and fill messages
        self.assertIn('Backfilled', out)
        self.assertIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 3)
        
        # @3 should be unchanged
        self.assertClosedInterval(j[0],
                                expectedStart=four_hours_before_utc,
                                expectedEnd=three_hour_thirty_before_utc,
                                expectedTags=['task_three'])
        
        # @2 should now span from end of @3 to start of @1
        self.assertClosedInterval(j[1],
                                expectedStart=three_hour_thirty_before_utc,  # end of @3
                                expectedEnd=one_hour_thirty_before_utc,     # start of @1
                                expectedTags=['task_two'])
        
        # @1 should be unchanged
        self.assertClosedInterval(j[2],
                                expectedStart=one_hour_thirty_before_utc,
                                expectedEnd=one_hour_before_utc,
                                expectedTags=['task_one'])

    def test_modify_range_with_fill_hint_no_adjacent_intervals(self):
        """Call modify range with :fill hint when there are no adjacent intervals"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        
        # Create a single isolated interval
        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_isolated".format(two_hours_before_utc, one_hour_before_utc))

        # Modify @1 with :fill hint - should not change anything since there are no adjacent intervals
        code, out, err = self.t("modify @1 range :fill")

        # Should not see backfill or fill messages
        self.assertNotIn('Backfilled', out)
        self.assertNotIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        
        # @1 should be unchanged since there are no adjacent intervals to fill to
        self.assertClosedInterval(j[0],
                                expectedStart=two_hours_before_utc,
                                expectedEnd=one_hour_before_utc,
                                expectedTags=['task_isolated'])

    def test_modify_start_with_fill_hint(self):
        """Call modify start with :fill hint to backfill start to adjacent interval"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        
        # Create two intervals with a gap between them:
        # @2: 3 hours ago for 30min (3:00 - 2:30)
        # @1: 2 hours ago for 30min (2:00 - 1:30) - gap from 2:30 to 2:00
        one_hour_thirty_before_utc = now_utc - timedelta(hours=1, minutes=30)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        two_hour_thirty_before_utc = now_utc - timedelta(hours=2, minutes=30)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        # Create intervals @2, @1 (in reverse chronological order)
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_two".format(three_hours_before_utc, two_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_one".format(two_hours_before_utc, one_hour_thirty_before_utc))

        # Modify @1 start with :fill hint - should backfill start to end of @2
        code, out, err = self.t("modify @1 start :fill")

        # Should see backfill message
        self.assertIn('Backfilled', out)

        j = self.t.export()

        self.assertEqual(len(j), 2)
        
        # @2 should be unchanged
        self.assertClosedInterval(j[0],
                                expectedStart=three_hours_before_utc,
                                expectedEnd=two_hour_thirty_before_utc,
                                expectedTags=['task_two'])
        
        # @1 should have its start backfilled to end of @2
        self.assertClosedInterval(j[1],
                                expectedStart=two_hour_thirty_before_utc,  # end of @2
                                expectedEnd=one_hour_thirty_before_utc,   # original end
                                expectedTags=['task_one'])

    def test_modify_end_with_fill_hint(self):
        """Call modify end with :fill hint to fill end to adjacent interval"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        
        # Create two intervals with a gap between them:
        # @2: 3 hours ago for 30min (3:00 - 2:30) - gap from 2:30 to 2:00
        # @1: 2 hours ago for 30min (2:00 - 1:30)
        one_hour_thirty_before_utc = now_utc - timedelta(hours=1, minutes=30)
        two_hours_before_utc = now_utc - timedelta(hours=2)
        two_hour_thirty_before_utc = now_utc - timedelta(hours=2, minutes=30)
        three_hours_before_utc = now_utc - timedelta(hours=3)

        # Create intervals @2, @1 (in reverse chronological order)
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_two".format(three_hours_before_utc, two_hour_thirty_before_utc))
        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_one".format(two_hours_before_utc, one_hour_thirty_before_utc))

        # Modify @2 end with :fill hint - should fill end to start of @1
        code, out, err = self.t("modify @2 end :fill")

        # Should see fill message
        self.assertIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 2)
        
        # @2 should have its end filled to start of @1
        self.assertClosedInterval(j[0],
                                expectedStart=three_hours_before_utc,    # original start
                                expectedEnd=two_hours_before_utc,        # start of @1
                                expectedTags=['task_two'])
        
        # @1 should be unchanged
        self.assertClosedInterval(j[1],
                                expectedStart=two_hours_before_utc,
                                expectedEnd=one_hour_thirty_before_utc,
                                expectedTags=['task_one'])

    def test_modify_start_with_fill_hint_no_previous_interval(self):
        """Call modify start with :fill hint when there is no previous interval"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        
        # Create a single isolated interval
        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_isolated".format(two_hours_before_utc, one_hour_before_utc))

        # Modify @1 start with :fill hint - should not change anything since there is no previous interval
        code, out, err = self.t("modify @1 start :fill")

        # Should not see backfill message
        self.assertNotIn('Backfilled', out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        
        # @1 should be unchanged since there is no previous interval
        self.assertClosedInterval(j[0],
                                expectedStart=two_hours_before_utc,
                                expectedEnd=one_hour_before_utc,
                                expectedTags=['task_isolated'])

    def test_modify_end_with_fill_hint_no_next_interval(self):
        """Call modify end with :fill hint when there is no next interval"""
        now = datetime.now().replace(second=0, microsecond=0, minute=0)
        now_utc = now.replace(tzinfo=tz.tzlocal()).astimezone(timezone.utc).replace(second=0, microsecond=0, minute=0)
        
        # Create a single isolated interval
        two_hours_before_utc = now_utc - timedelta(hours=2)
        one_hour_before_utc = now_utc - timedelta(hours=1)

        self.t("track {:%Y-%m-%dT%H:%M:%S}Z - {:%Y-%m-%dT%H:%M:%S}Z task_isolated".format(two_hours_before_utc, one_hour_before_utc))

        # Modify @1 end with :fill hint - should not change anything since there is no next interval
        code, out, err = self.t("modify @1 end :fill")

        # Should not see fill message
        self.assertNotIn('Filled', out)

        j = self.t.export()

        self.assertEqual(len(j), 1)
        
        # @1 should be unchanged since there is no next interval
        self.assertClosedInterval(j[0],
                                expectedStart=two_hours_before_utc,
                                expectedEnd=one_hour_before_utc,
                                expectedTags=['task_isolated'])

if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
