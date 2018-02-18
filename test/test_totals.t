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

import os
import subprocess
import sys
import unittest

import datetime

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import TestCase


class TestTotals(TestCase):
    def setUp(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.process = subprocess.Popen([os.path.join(current_dir, '../ext/totals.py')],
                                        stdin=subprocess.PIPE,
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE)

    def test_totals_with_empty_database(self):
        """totals extension should report error on empty database"""
        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start:
temp.report.end:

[
]\
""")

        self.assertEqual('There is no data in the database\n', out)
        self.assertEqual('', err)

    def test_totals_with_filled_database(self):
        """totals extension should print report for filled database"""
        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start: 20160101T070000Z
temp.report.end: 20160101T080000Z

[
{"start":"20160101T070000Z","end":"20160101T080000Z","tags":["foo"]}
]\
""")

        self.assertEqual('\nTotal by Tag, for 2016-01-01 07:00:00 - 2016-01-01 08:00:00\n\nTag        Total\n----- ----------\nfoo      1:00:00\n      ----------\nTotal    1:00:00\n', out)
        self.assertEqual('', err)

    def test_totals_with_interval_without_tags(self):
        """totals extension should handle interval without tags"""
        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start: 20160101T070000Z
temp.report.end: 20160101T080000Z

[{"start":"20160101T070000Z","end":"20160101T080000Z"}]\
""")

        self.assertEqual('\nTotal by Tag, for 2016-01-01 07:00:00 - 2016-01-01 08:00:00\n\nTag        Total\n----- ----------\n         1:00:00\n      ----------\nTotal    1:00:00\n', out)
        self.assertEqual('', err)

    def test_totals_with_interval_with_empty_tag_list(self):
        """totals extension should handle interval with empty tag list"""
        out, err = self.process.communicate(input="""\
color: off
debug: off
temp.report.start: 20160101T070000Z
temp.report.end: 20160101T080000Z

[
{"start":"20160101T070000Z","end":"20160101T080000Z","tags":[]}
]\
""")

        self.assertEqual('\nTotal by Tag, for 2016-01-01 07:00:00 - 2016-01-01 08:00:00\n\nTag        Total\n----- ----------\n         1:00:00\n      ----------\nTotal    1:00:00\n', out)
        self.assertEqual('', err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
