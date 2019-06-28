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

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestConfig(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def test_trivial_config(self):
        """Test trivial config - should only show defaults"""
        code, out, err = self.t("config")
        # Note that this is not a complete set, just a representative set.
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
        code, out, err = self.t("config")
        self.assertIn("name = value", out)

        # Should fail with exit 1, because the value does not change.
        code, out, err = self.t.runError("config name value :yes")
        self.assertIs(code, 1)

    def test_set_new_name_blank_value(self):
        """Test setting a new name, blank value"""
        code, out, err = self.t("config name '' :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("name = ", out)

    def test_unset_new_name(self):
        """Test unsetting a new name, no value"""
        code, out, err = self.t("config name value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config name :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertNotIn("name = ", out)

    def test_set_new_hierarchical_name_new_value(self):
        """Test setting a new hierarchical name, new value"""
        code, out, err = self.t("config foo.bar.baz value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("baz = value", out)

    def test_set_new_hierarchical_name_same_value(self):
        """Test setting a new hierarchical name, same value"""
        code, out, err = self.t("config foo.bar.baz value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')
        code, out, err = self.t("config")
        self.assertIn("baz = value", out)

        # Should fail with exit 1, because the value does not change.
        code, out, err = self.t.runError("config foo.bar.baz value :yes")
        self.assertIs(code, 1)

    def test_set_new_hierarchical_name_blank_value(self):
        """Test setting a new hierarchical name, blank value"""
        code, out, err = self.t("config foo.bar.baz '' :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("baz = ", out)

    def test_unset_new_hierarchical_name(self):
        """Test unsetting a new hierarchical name, no value"""
        code, out, err = self.t("config foo.bar.baz value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config foo.bar.baz :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertNotIn("baz = ", out)

    def test_set_known_name(self):
        """Test setting a known name, new value"""
        code, out, err = self.t("config debug value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("debug = value", out)

    def test_set_known_name_same_value(self):
        """Test setting a known name, same value"""
        code, out, err = self.t("config debug value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')
        code, out, err = self.t("config")
        self.assertIn("debug = value", out)

        # Should fail with exit 1, because the value does not change.
        code, out, err = self.t.runError("config debug value :yes")
        self.assertIs(code, 1)

    def test_set_known_name_blank_value(self):
        """Test setting a known name, blank value"""
        code, out, err = self.t("config debug '' :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("debug = ", out)

    def test_unset_known_name(self):
        """Test unsetting a known name"""
        code, out, err = self.t.runError("config debug :yes")
        self.assertRegexpMatches(err, r"No entry named 'debug' found\.\n$")

    def test_reset_known_name(self):
        """Test setting a known name"""
        code, out, err = self.t("config debug foo :yes")

        code, out, err = self.t("config debug :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

    def test_set_known_hierarchical_name(self):
        """Test setting a known hierarchical name, new value"""
        code, out, err = self.t("config reports.day.month value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("month = value", out)

    def test_set_known_hierarchical_name_same_value(self):
        """Test setting a known hierarchical name, same value"""
        code, out, err = self.t("config reports.day.month value :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')
        code, out, err = self.t("config")
        self.assertIn("month = value", out)

        # Should fail with exit 1, because the value does not change.
        code, out, err = self.t.runError("config reports.day.month value :yes")
        self.assertIs(code, 1)

    def test_set_known_hierarchical_name_blank_value(self):
        """Test setting a known hierarchical name, blank value"""
        code, out, err = self.t("config reports.day.month '' :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

        code, out, err = self.t("config")
        self.assertIn("month = ", out)

    def test_unset_known_hierarchical_name(self):
        """Test unsetting a known hierarchical name"""
        code, out, err = self.t.runError("config reports.day.month :yes")
        self.assertRegexpMatches(err, r"^No entry named 'reports.day.month' found\.\n$")

    def test_reset_known_hierarchical_name(self):
        """Test resetting a known hierarchical name"""
        code, out, err = self.t("config reports.day.month foo :yes")

        code, out, err = self.t("config reports.day.month :yes")
        self.assertRegexpMatches(out, r'^Config file .+ modified\.$')

    def test_number_to_date_upgrade(self):
        """Test that an integer remains an integer"""
        code, out, err = self.t("config number 5", input="yes\n")
        self.assertIn(" with a value of '5'?", out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())

# vim: ai sts=4 et sw=4 ft=python
