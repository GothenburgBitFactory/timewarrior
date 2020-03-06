import datetime
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
                           expectedId=None,
                           expectedStart=None,
                           expectedTags=None,
                           expectedAnnotation=None,
                           description="interval"):
        self.assertKeyExists(interval, "start", description, "{} does not contain a start date")
        self.assertKeyNotExists(interval, "end", description, "{} does contain an end date")

        return self.assertInterval(interval,
                                   expectedId=expectedId,
                                   expectedStart=expectedStart,
                                   expectedEnd=None,
                                   expectedTags=expectedTags,
                                   expectedAnnotation=expectedAnnotation,
                                   description=description)

    def assertClosedInterval(self, interval,
                             expectedId=None,
                             expectedStart=None,
                             expectedEnd=None,
                             expectedTags=None,
                             expectedAnnotation=None,
                             description="interval"):
        self.assertKeyExists(interval, "start", description, "{} does not contain a start date")
        self.assertKeyExists(interval, "end", description, "{} does not contain an end date")

        return self.assertInterval(interval,
                                   expectedId=expectedId,
                                   expectedStart=expectedStart,
                                   expectedEnd=expectedEnd,
                                   expectedTags=expectedTags,
                                   expectedAnnotation=expectedAnnotation,
                                   description=description)

    def assertInterval(self, interval,
                       expectedId=None,
                       expectedStart=None,
                       expectedEnd=None,
                       expectedTags=None,
                       expectedAnnotation=None,
                       description="interval"):
        if expectedId is not None:
            self.assertKeyExists(interval, "id", description, "{} does not contain an id")
            self.assertIntervalValue(interval,
                                     "id",
                                     expectedId,
                                     description,
                                     "{} of {} do not match (expected: '{}', actual: '{}')")

        if expectedStart:
            self.assertIntervalTimestamp(interval, "start", expectedStart, description)

        if expectedEnd:
            self.assertIntervalTimestamp(interval, "end", expectedEnd, description)

        if expectedTags:
            self.assertKeyExists(interval, "tags", description, "{} does not contain tags")
            self.assertIntervalValue(interval,
                                     "tags",
                                     expectedTags,
                                     description,
                                     "{} of {} do not match (expected: '{}', actual: '{}')")

        if expectedAnnotation:
            self.assertKeyExists(interval, "annotation", description, "{} is not annotated")
            self.assertIntervalValue(interval,
                                     "annotation",
                                     expectedAnnotation,
                                     description,
                                     "{} of {} do not match (expected: '{}', actual: '{}')")

    def assertKeyExists(self, interval, key, description, message):
        self.assertTrue(key in interval, message.format(description))

    def assertKeyNotExists(self, interval, key, description, message):
        self.assertFalse(key in interval, message.format(description))

    def assertIntervalTimestamp(self, interval, key, expected, description):
        if isinstance(expected, datetime.datetime):
            expected = "{:%Y%m%dT%H%M%SZ}".format(expected)

        self.assertIntervalValue(interval,
                                 key,
                                 expected,
                                 description,
                                 "{} time of {} does not match (expected: '{}', actual: '{}')")

    def assertIntervalValue(self, interval, key, expected, description, message):
        actual = interval[key]

        if isinstance(actual, list):
            actual.sort()
            expected.sort()
            self.assertSequenceEqual(actual,
                                     expected,
                                     message.format(key, description, expected, actual))
        else:
            self.assertEqual(actual,
                             expected,
                             message.format(key, description, expected, actual))
