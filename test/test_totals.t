#!/usr/bin/env python2
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

import datetime

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'ext'))

from basetest import TestCase
from totals import *


class TestTotals(TestCase):
    def test_totals_with_empty_database(self):
        """totals extension should report error on empty database"""
        input_stream = [
            'color: off\n',
            'debug: on\n',
            'temp.report.start: \n',
            'temp.report.end: \n',
            '\n',
            '[]',
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(['There is no data in the database'], out)

    def test_totals_with_filled_database(self):
        """totals extension should print report for filled database"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: off\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[{{"start":"{:%Y%m%dT%H%M%S}Z","end":"{:%Y%m%dT%H%M%S}Z","tags":["foo"]}}]'.format(one_hour_before_utc, now_utc)
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                'Tag        Total',
                '----- ----------',
                'foo      1:00:00',
                '      ----------',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_totals_with_emtpy_range(self):
        """totals extension should report error on emtpy range"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: off\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[]',
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(['No data in the range {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now)], out)

    def test_totals_with_interval_without_tags(self):
        """totals extension should handle interval without tags"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: off\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[{{"start":"{:%Y%m%dT%H%M%S}Z","end":"{:%Y%m%dT%H%M%S}Z"}}]'.format(one_hour_before_utc, now_utc)
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                'Tag        Total',
                '----- ----------',
                '         1:00:00',
                '      ----------',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_totals_with_interval_with_empty_tag_list(self):
        """totals extension should handle interval with empty tag list"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: off\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[{{"start":"{:%Y%m%dT%H%M%S}Z","end":"{:%Y%m%dT%H%M%S}Z","tags":[]}}]'.format(one_hour_before_utc, now_utc)
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                'Tag        Total',
                '----- ----------',
                '         1:00:00',
                '      ----------',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_totals_with_open_interval(self):
        """totals extension should handle open interval"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: off\n',
            'debug: off\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: \n',
            '\n',
            '[{{"start":"{:%Y%m%dT%H%M%S}Z","tags":["foo"]}}]'.format(one_hour_before_utc),
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                'Tag        Total',
                '----- ----------',
                'foo      1:00:00',
                '      ----------',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_totals_colored_with_empty_database(self):
        """totals extension should report error on empty database (colored)"""
        input_stream = [
            'color: on\n',
            'debug: on\n',
            'temp.report.start: \n',
            'temp.report.end: \n',
            '\n',
            '[]',
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(['There is no data in the database'], out)

    def test_totals_colored_with_filled_database(self):
        """totals extension should print report for filled database (colored)"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: on\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[{{"start":"{:%Y%m%dT%H%M%S}Z","end":"{:%Y%m%dT%H%M%S}Z","tags":["foo"]}}]'.format(one_hour_before_utc, now_utc)
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                '[4mTag  [0m [4m     Total[0m',
                'foo      1:00:00',
                '      [4m          [0m',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_totals_colored_with_emtpy_range(self):
        """totals extension should report error on emtpy range (colored)"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: on\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[]',
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(['No data in the range {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now)], out)

    def test_totals_colored_with_interval_without_tags(self):
        """totals extension should handle interval without tags (colored)"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: on\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[{{"start":"{:%Y%m%dT%H%M%S}Z","end":"{:%Y%m%dT%H%M%S}Z"}}]'.format(one_hour_before_utc, now_utc)
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                '[4mTag  [0m [4m     Total[0m',
                '         1:00:00',
                '      [4m          [0m',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_totals_colored_with_interval_with_empty_tag_list(self):
        """totals extension should handle interval with empty tag list (colored)"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: on\n',
            'debug: on\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: {:%Y%m%dT%H%M%S}Z\n'.format(now_utc),
            '\n',
            '[{"start":"20160101T070000Z","end":"20160101T080000Z","tags":[]}]',
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                '[4mTag  [0m [4m     Total[0m',
                '         1:00:00',
                '      [4m          [0m',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_totals_colored_with_open_interval(self):
        """totals extension should handle open interval (colored)"""
        now = datetime.datetime.now()
        one_hour_before = now - datetime.timedelta(hours=1)

        now_utc = now.utcnow()
        one_hour_before_utc = now_utc - datetime.timedelta(hours=1)

        input_stream = [
            'color: on\n',
            'debug: off\n',
            'temp.report.start: {:%Y%m%dT%H%M%S}Z\n'.format(one_hour_before_utc),
            'temp.report.end: \n',
            '\n',
            '[{{"start":"{:%Y%m%dT%H%M%S}Z","tags":["foo"]}}]'.format(one_hour_before_utc),
        ]

        out = calculate_totals(input_stream)

        self.assertEqual(
            [
                '',
                'Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}'.format(one_hour_before, now),
                '',
                '[4mTag  [0m [4m     Total[0m',
                'foo      1:00:00',
                '      [4m          [0m',
                'Total    1:00:00',
                '',
            ],
            out)

    def test_format_seconds_with_less_than_1_minute(self):
        """Test format_seconds with less than 1 minute"""
        self.assertEqual(format_seconds(34), '   0:00:34')

    def test_format_seconds_with_1_minute(self):
        """Test format_seconds with 1 minute"""
        self.assertEqual(format_seconds(60), '   0:01:00')

    def test_format_seconds_with_1_hour(self):
        """Test format_seconds with 1 hour"""
        self.assertEqual(format_seconds(3600), '   1:00:00')

    def test_format_seconds_with_more_than_1_hour(self):
        """Test format_seconds with more than 1 hour"""
        self.assertEqual(format_seconds(3645), '   1:00:45')


if __name__ == '__main__':
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
