#!/usr/bin/env python3

###############################################################################
#
# Copyright 2025, Gothenburg Bit Factory.
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
# https://opensource.org/license/mit
#
###############################################################################

import os
import stat
import sys
import unittest

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestHolidayProvider(TestCase):
    def setUp(self):
        """Executed before each test in the class"""
        self.t = Timew()

    def _make_provider(self, lines):
        """Create a shell script provider returning the given JSON lines."""
        provider = os.path.join(self.t.datadir, "holiday_provider.sh")
        with open(provider, "w") as f:
            f.write("#!/bin/sh\n")
            for line in lines:
                f.write("echo '{}'\n".format(line))
        os.chmod(provider, stat.S_IRWXU | stat.S_IRGRP | stat.S_IROTH)
        return provider

    def test_provider_holidays_appear_in_summary(self):
        """holidays.provider output appears in summary report"""
        provider = self._make_provider([
            '{"date":"2025-01-01","name":"New Year Day"}'
        ])
        self.t.config("holidays.provider", provider)
        self.t("track 2025-01-01T10:00:00 - 2025-01-01T11:00:00")

        code, out, err = self.t("summary 2025-01-01 :holidays")
        self.assertIn("New Year Day", out)

    def test_provider_overrides_config_holidays(self):
        """holidays.provider takes precedence over config-file holidays"""
        # The config key uses underscores throughout: the locale portion uses
        # underscore (en_US) because the Lexer treats '-' as an operator, and
        # the date portion always uses underscores (2025_01_01) as required by
        # the timewarrior config date key format.
        self.t.config("holidays.en_US.2025_01_01", "Config New Year")

        # Configure a provider that emits a different name
        provider = self._make_provider([
            '{"date":"2025-01-01","name":"Provider New Year"}'
        ])
        self.t.config("holidays.provider", provider)
        self.t("track 2025-01-01T10:00:00 - 2025-01-01T11:00:00")

        code, out, err = self.t("summary 2025-01-01 :holidays")
        self.assertIn("Provider New Year", out)
        self.assertNotIn("Config New Year", out)

    def test_config_holidays_used_when_no_provider(self):
        """Config-file holidays are used when holidays.provider is not set"""
        # The config key uses underscores throughout: the locale portion uses
        # underscore (en_US) because the Lexer treats '-' as an operator, and
        # the date portion always uses underscores (2025_01_01) as required by
        # the timewarrior config date key format.
        self.t.config("holidays.en_US.2025_01_01", "Config New Year")
        self.t("track 2025-01-01T10:00:00 - 2025-01-01T11:00:00")

        code, out, err = self.t("summary 2025-01-01 :holidays")
        self.assertIn("Config New Year", out)

    def test_provider_receives_date_range_arguments(self):
        """holidays.provider is called with start and end date as arguments"""
        provider = os.path.join(self.t.datadir, "holiday_provider.sh")
        args_file = os.path.join(self.t.datadir, "provider_args.txt")
        with open(provider, "w") as f:
            f.write("#!/bin/sh\n")
            f.write("echo \"$1 $2\" > {}\n".format(args_file))
        os.chmod(provider, stat.S_IRWXU | stat.S_IRGRP | stat.S_IROTH)

        self.t.config("holidays.provider", provider)
        self.t("track 2025-06-15T10:00:00 - 2025-06-15T11:00:00")
        self.t("summary 2025-06-15 :holidays")

        with open(args_file) as f:
            args = f.read().strip()

        # The provider must be called with the start and end date of the range
        self.assertIn("2025-06-15", args)

    def test_provider_malformed_lines_skipped(self):
        """Malformed JSON lines from the provider are silently skipped"""
        provider = self._make_provider([
            "not json at all",
            '{"date":"2025-01-01","name":"Good Holiday"}',
            '{"incomplete":',
        ])
        self.t.config("holidays.provider", provider)
        self.t("track 2025-01-01T10:00:00 - 2025-01-01T11:00:00")

        # Should not crash; valid entry should appear
        code, out, err = self.t("summary 2025-01-01 :holidays")
        self.assertIn("Good Holiday", out)


if __name__ == "__main__":
    from simpletap import TAPTestRunner

    unittest.main(testRunner=TAPTestRunner())
