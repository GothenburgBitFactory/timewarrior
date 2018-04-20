# -*- coding: utf-8 -*-

import sys
import unittest


class BaseTestCase(unittest.TestCase):
    def tap(self, out):
        sys.stderr.write("--- tap output start ---\n")
        for line in out.splitlines():
            sys.stderr.write(line + '\n')
        sys.stderr.write("---  tap output end  ---\n")


class TestCase(BaseTestCase):
    def assertInterval(self, interval, expectedStart=None, expectedEnd=None, expectedTags=None, description=None):
        self.assertTrue('start' in interval)
        self.assertTrue('end' in interval)

        if expectedStart:
            self.assertEqual(interval['start'], expectedStart, description)

        if expectedEnd:
            self.assertEqual(interval['end'], expectedEnd, description)

        if expectedTags:
            self.assertTrue('tags' in interval)
            self.assertEqual(interval['tags'], expectedTags, description)


# vim: ai sts=4 et sw=4
