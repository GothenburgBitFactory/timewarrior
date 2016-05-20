#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
###############################################################################
#
# Copyright 2006 - 2016, Paul Beckingham, Federico Hernandez.
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

class TestConfig(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_config(self):
        """Test trivial config - should only show defaults"""
        code, out, err = self.t("config")
        # Note that this is not a complete set, just a representative set.
        self.assertIn("color = off", out)
        self.assertIn("confirmation = on", out)
        self.assertIn("debug = off", out)
        self.assertIn("verbose = on", out)

    def test_set_new_name_new_value(self):
        """Test setting a new name, new value"""
        code, out, err = self.t("config name value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("name = value", out)

    def test_set_new_name_new_value_internal(self):
        """Test setting a new name, new value internal"""
        self.t.config("name", "value")
        code, out, err = self.t("config")
        self.assertIn("name = value", out)

    def test_set_new_name_same_value(self):
        """Test setting a new name, same value"""
        code, out, err = self.t("config name value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        # Should fail with exit 1
        code, out, err = self.t.runError("config name value :yes")
        self.assertIs(code, 1)

    def test_set_new_name_blank_value(self):
        """Test setting a new name, blank value"""
        code, out, err = self.t("config name '' :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("name = ", out)

    def test_set_new_name_no_value(self):
        """Test setting a new name, no value"""
        code, out, err = self.t("config name value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config name :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertNotIn("name = ", out)

if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
