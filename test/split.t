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

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestSplit(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_split_closed_interval(self):
        """Split a closed interval"""
        self.t("track 2016-01-01T00:00:00 - 2016-01-01T01:00:00 foo")
        code, out, err = self.t("split @1")
        self.assertIn('Split @1', out)

        j = self.t.export()
        self.assertEqual(len(j), 2)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertTrue('end' in j[1])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'][0], 'foo')

        self.assertEqual(j[0]['end'], j[1]['start'])

    def test_split_open_interval(self):
        """Split an open interval"""
        self.t("start 5mins ago foo")
        code, out, err = self.t("split @1")
        self.assertIn('Split @1', out)

        j = self.t.export()
        self.assertEqual(len(j), 2)
        self.assertTrue('start' in j[0])
        self.assertTrue('end' in j[0])
        self.assertTrue('tags' in j[0])
        self.assertEqual(j[0]['tags'][0], 'foo')

        self.assertTrue('start' in j[1])
        self.assertTrue('end' not in j[1])
        self.assertTrue('tags' in j[1])
        self.assertEqual(j[1]['tags'][0], 'foo')

        self.assertEqual(j[0]['end'], j[1]['start'])


# TODO Add :adjust tests.


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
