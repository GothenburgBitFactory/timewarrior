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
    def assertOpenInterval(self, interval,
                           expectedStart=None,
                           expectedTags=None,
                           description="interval"):
        self.assertTrue("start" in interval, "{} does not contain a start date".format(description))
        self.assertFalse("end" in interval, "{} does contain an end date".format(description))

        return self.assertInterval(interval,
                                   expectedStart=expectedStart,
                                   expectedEnd=None,
                                   expectedTags=expectedTags,
                                   description=description)

    def assertClosedInterval(self, interval,
                             expectedStart=None,
                             expectedEnd=None,
                             expectedTags=None,
                             description="interval"):
        self.assertTrue("start" in interval, "{} does not contain a start date".format(description))
        self.assertTrue("end" in interval, "{} does not contain an end date".format(description))

        return self.assertInterval(interval,
                                   expectedStart=expectedStart,
                                   expectedEnd=expectedEnd,
                                   expectedTags=expectedTags,
                                   description=description)

    def assertInterval(self, interval,
                       expectedStart=None,
                       expectedEnd=None,
                       expectedTags=None,
                       description="interval"):
        if expectedStart:
            self.assertEqual(
                interval["start"],
                expectedStart,
                "start time of {} does not match (expected: {}, actual: {})".format(description,
                                                                                    expectedStart,
                                                                                    interval["start"]))

        if expectedEnd:
            self.assertEqual(
                interval["end"],
                expectedEnd,
                "end time of {} does not match (expected: {}, actual: {})".format(description,
                                                                                  expectedEnd,
                                                                                  interval["end"]))

        if expectedTags:
            self.assertTrue("tags" in interval)
            self.assertEqual(
                interval["tags"],
                expectedTags,
                "tags of {} do not match (expected: {}, actual: {})". format(description,
                                                                             expectedTags,
                                                                             interval["tags"]))


# vim: ai sts=4 et sw=4
