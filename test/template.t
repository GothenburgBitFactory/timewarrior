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


class TestBugNumber(TestCase):
    @classmethod
    def setUpClass(cls):
        """Executed once before any test in the class"""
        # Used to initialize objects that can be shared across tests
        # Also useful if none of the tests of the current TestCase performs
        # data alterations. See tw-285.t for an example

    def setUp(self):
        """Executed before each test in the class"""
        # Used to initialize objects that should be re-initialized or
        # re-created for each individual test
        self.t = Timew()

    def test_foo(self):
        """Test foo"""
        code, out, err = self.t("foo")
        self.tap(out)
        self.tap(err)

    def test_faketime(self):
        """Running tests using libfaketime

           WARNING:
             faketime version 0.9.6 and later correctly propagates non-zero
             exit codes.  Please don't combine faketime tests and
             self.t.runError().

             https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=750721
        """
        self.t.faketime("-2y")

        command = ("insert test here")
        self.t(command)

        # Remove FAKETIME settings
        self.t.faketime()

        code, out, err = self.t("insert test here")
        expected = "2.0y"
        self.assertIn(expected, out)

    @unittest.skipIf(1 != 0, "This machine has sane logic")
    def test_skipped(self):
        """Test all logic of the world"""

    @unittest.expectedFailure
    def test_expected_failure(self):
        """Test something that fails and we know or expect that"""
        self.assertEqual(1, 0)

    def tearDown(self):
        """Executed after each test in the class"""

    @classmethod
    def tearDownClass(cls):
        """Executed once after all tests in the class"""


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
