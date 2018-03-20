#!/usr/bin/env python2.7
###############################################################################
#
# Copyright 2006 - 2018, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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
import subprocess
import sys
import unittest

# Ensure python finds the local simpletap module
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from basetest import Timew, TestCase


class TestOnModifyHookScript(TestCase):
    def setUp(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.t = Timew()

        self.process = subprocess.Popen([os.path.join(current_dir, '../ext/on-modify.timewarrior')],
                                        env={
                                            'PATH': '../src:' + os.environ['PATH'],
                                            'TIMEWARRIORDB': self.t.datadir
                                        },
                                        stdin=subprocess.PIPE,
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE)

    def test_hook_should_process_annotate(self):
        """on-modify hook should process 'task annotate'"""
        out, err = self.process.communicate(input="""\
{ "description": "Foo", "entry": "20180316T231055Z", "modified": "20180316T231055Z", "status": "pending", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3" }
{ "description": "Foo", "entry": "20180316T231055Z", "modified": "20180316T231055Z", "status": "pending", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3", "annotations": [{ "entry": "20180316T231239Z", "description": "Bar" }]}
""")

        self.assertEqual('{"status": "pending", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3", "modified": "20180316T231055Z", "entry": "20180316T231055Z", "annotations": [{"entry": "20180316T231239Z", "description": "Bar"}], "description": "Foo"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_append(self):
        """on-modify hook should process 'task append'"""
        out, err = self.process.communicate(input="""\
{ "description": "FooBar", "entry": "20180316T231125Z", "modified": "20180316T231808Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
{ "description": "FooBar Baz", "entry": "20180316T231125Z", "modified": "20180316T231808Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
""")

        self.assertEqual('{"status": "pending", "entry": "20180316T231125Z", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094", "description": "FooBar Baz", "modified": "20180316T231808Z"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_delete(self):
        """on-modify hook should process 'task delete'"""
        out, err = self.process.communicate(input="""\
{ "description": "Read Taskwarrior documents later", "entry": "20180316T225357Z", "modified": "20180316T225357Z", "status": "pending", "uuid": "3bfe7518-8644-4c3c-9788-940d782fefac" }
{ "description": "Read Taskwarrior documents later", "end": "20180316T230837Z", "entry": "20180316T225357Z", "modified": "20180316T225357Z", "status": "deleted", "uuid": "3bfe7518-8644-4c3c-9788-940d782fefac" }
""")

        self.assertEqual('{"status": "deleted", "end": "20180316T230837Z", "uuid": "3bfe7518-8644-4c3c-9788-940d782fefac", "modified": "20180316T225357Z", "entry": "20180316T225357Z", "description": "Read Taskwarrior documents later"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_denotate(self):
        """on-modify hook should process 'task denotate'"""
        out, err = self.process.communicate(input="""\
{ "description": "Foo", "entry": "20180316T231055Z", "modified": "20180316T231239Z", "status": "pending", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3", "annotations": [{ "entry": "20180316T231239Z", "description": "Bar" }]}
{ "description": "Foo", "entry": "20180316T231055Z", "modified": "20180316T231239Z", "status": "pending", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3" }
""")

        self.assertEqual('{"status": "pending", "entry": "20180316T231055Z", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3", "description": "Foo", "modified": "20180316T231239Z"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_done(self):
        """on-modify hook should process 'task done'"""
        out, err = self.process.communicate(input="""\
{ "description": "Pay bills", "entry": "20180316T225418Z", "modified": "20180316T225418Z", "priority": "H", "status": "pending", "uuid": "94699c4e-b1ae-4838-bc6b-c8476251f898" }
{ "description": "Pay bills", "end": "20180316T230221Z", "entry": "20180316T225418Z", "modified": "20180316T225418Z", "priority": "H", "status": "completed", "uuid": "94699c4e-b1ae-4838-bc6b-c8476251f898" }
""")

        self.assertEqual('{"status": "completed", "end": "20180316T230221Z", "description": "Pay bills", "modified": "20180316T225418Z", "priority": "H", "entry": "20180316T225418Z", "uuid": "94699c4e-b1ae-4838-bc6b-c8476251f898"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_edit(self):
        """on-modify hook should process 'task edit'"""
        out, err = self.process.communicate(input="""\
{ "description": "Foo", "entry": "20180316T231125Z", "modified": "20180316T231055Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
{ "description": "FooBar", "entry": "20180316T231125Z", "modified": "20180316T231055Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
""")

        self.assertEqual('{"status": "pending", "entry": "20180316T231125Z", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094", "description": "FooBar", "modified": "20180316T231055Z"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_modify(self):
        """on-modify hook should process 'task modify'"""
        out, err = self.process.communicate(input="""\
{"description":"dummy","entry":"20180317T092629Z","modified":"20180317T092629Z","start":"20180317T092629Z","status":"pending","uuid":"3422d76c-c087-4ecd-9c62-1246b078e534"}
{"description":"foo","entry":"20180317T092629Z","modified":"20180317T092629Z","start":"20180317T092629Z","status":"pending","uuid":"3422d76c-c087-4ecd-9c62-1246b078e534"}
""")

        self.assertEqual('{"status": "pending", "uuid": "3422d76c-c087-4ecd-9c62-1246b078e534", "modified": "20180317T092629Z", "start": "20180317T092629Z", "entry": "20180317T092629Z", "description": "foo"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_prepend(self):
        """on-modify hook should process 'task prepend'"""
        out, err = self.process.communicate(input="""\
{ "description": "Foo", "entry": "20180316T231055Z", "modified": "20180316T231617Z", "status": "pending", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3" }
{ "description": "Hey Foo", "entry": "20180316T231055Z", "modified": "20180316T231617Z", "status": "pending", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3" }
""")

        self.assertEqual('{"status": "pending", "entry": "20180316T231055Z", "uuid": "d07b40c4-5df7-409f-bf98-930e550333b3", "description": "Hey Foo", "modified": "20180316T231617Z"}\n', out)
        self.assertEqual('', err)

    def test_hook_should_process_start(self):
        """on-modify hook should process 'task start'"""
        out, err = self.process.communicate(input="""\
{ "description": "FooBar", "due": "20180331T220000Z", "entry": "20180316T231125Z", "modified": "20180316T232513Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
{ "description": "FooBar", "due": "20180331T220000Z", "entry": "20180316T231125Z", "modified": "20180316T232513Z", "start": "20180316T232651Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
""")

        self.assertRegexpMatches(out, """\
Tracking FooBar
  Started \d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}
  Current               (\d{2}:|   )\d{2}
  Total                ?\d{1,2}:\d{2}:\d{2}
\{"status": "pending", "description": "FooBar", "modified": "20180316T232513Z", "due": "20180331T220000Z", "start": "20180316T232651Z", "entry": "20180316T231125Z", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094"\}
""")
        self.assertEqual('', err)

    def test_hook_should_process_stop(self):
        """on-modify hook should process 'task stop'"""
        self.t("start 10min ago")
        out, err = self.process.communicate(input="""\
{ "description": "FooBar", "due": "20180331T220000Z", "entry": "20180316T231125Z", "modified": "20180316T232651Z", "start": "20180316T232651Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
{ "description": "FooBar", "due": "20180331T220000Z", "entry": "20180316T231125Z", "modified": "20180316T232651Z", "status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094" }
""")

        self.assertRegexpMatches(out, """\
Recorded 
  Started \d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}
  Ended                 (\d{2}:|   )\d{2}
  Total               ?\d{1,2}:\d{2}:\d{2}
\{"status": "pending", "uuid": "e11d7d19-20a7-47bb-999e-6554a70ea094", "modified": "20180316T232651Z", "due": "20180331T220000Z", "entry": "20180316T231125Z", "description": "FooBar"\}
""")
        self.assertEqual('', err)


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())
