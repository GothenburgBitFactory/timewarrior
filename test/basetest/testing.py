# -*- coding: utf-8 -*-

import unittest
import sys

class BaseTestCase(unittest.TestCase):
    def tap(self, out):
        sys.stderr.write("--- tap output start ---\n")
        for line in out.splitlines():
            sys.stderr.write(line + '\n')
        sys.stderr.write("---  tap output end  ---\n")

class TestCase(BaseTestCase):
    pass

# vim: ai sts=4 et sw=4
