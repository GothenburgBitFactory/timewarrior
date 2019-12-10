#!/usr/bin/env python3

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
                                        shell=True,
                                        stdin=subprocess.PIPE,
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE)

    def test_hook_should_process_annotate(self):
        """on-modify hook should process 'task annotate'"""
        self.t("start 10min ago Foo")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"pending","uuid":"3495a755-c4c6-4106-aabe-c0d3d128b65a"}
{"description":"Foo","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"pending","uuid":"3495a755-c4c6-4106-aabe-c0d3d128b65a","annotations":[{"entry":"20190820T201911Z","description":"Annotation"}]}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Foo"], expectedAnnotation="Annotation")

    def test_hook_should_process_append(self):
        """on-modify hook should process 'task append'"""
        self.t("start 10min ago Foo")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"pending","uuid":"da603270-ce2b-4a5a-9273-c67c2d2d0067"}
{"description":"Foo Bar","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"pending","uuid":"da603270-ce2b-4a5a-9273-c67c2d2d0067"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Foo Bar"])

    def test_hook_should_process_delete(self):
        """on-modify hook should process 'task delete'"""
        self.t("start 10min ago Foo")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"pending","uuid":"25b66283-96e0-42b4-b835-8efd0ea1043c"}
{"description":"Foo","end":"20190820T201911Z","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"deleted","uuid":"25b66283-96e0-42b4-b835-8efd0ea1043c"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["Foo"])

    def test_hook_should_process_denotate(self):
        """on-modify hook should process 'task denotate'"""
        self.t("start 10min ago Foo")
        self.t("annotate @1 Annotation")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"pending","uuid":"8811cc93-a495-4fa6-993e-2b96cffc48e0","annotations":[{"entry":"20190820T201911Z","description":"Annotation"}]}
{"description":"Foo","entry":"20190820T201911Z","modified":"20190820T201911Z","start":"20190820T201911Z","status":"pending","uuid":"8811cc93-a495-4fa6-993e-2b96cffc48e0"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Foo"], expectedAnnotation="")

    def test_hook_should_process_done(self):
        """on-modify hook should process 'task done'"""
        self.t("start 10min ago Foo")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T201912Z","modified":"20190820T201912Z","start":"20190820T201912Z","status":"pending","uuid":"c418b958-5c3c-4633-89a4-4a2f678d74d0"}
{"description":"Foo","end":"20190820T201912Z","entry":"20190820T201912Z","modified":"20190820T201912Z","status":"completed","uuid":"c418b958-5c3c-4633-89a4-4a2f678d74d0"}
""")

        self.assertEqual(b'', err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["Foo"])

    def test_hook_should_process_modify_desc(self):
        """on-modify hook should process 'task modify' for changing description"""
        self.t("start 10min ago Foo")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T203416Z","modified":"20190820T203416Z","start":"20190820T203416Z","status":"pending","uuid":"189e6745-04e0-4b17-949f-900cf63ab8d9"}
{"description":"Bar","entry":"20190820T203416Z","modified":"20190820T203416Z","start":"20190820T203416Z","status":"pending","uuid":"189e6745-04e0-4b17-949f-900cf63ab8d9"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Bar"])

    def test_hook_should_process_modify_tags(self):
        """on-modify hook should process 'task modify' for changing tags"""
        self.t("start 10min ago Foo Tag Bar")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T203620Z","modified":"20190820T203620Z","start":"20190820T203620Z","status":"pending","tags":["Tag","Bar"],"uuid":"6cab88f0-ac12-4a87-995a-0e7d39810c05"}
{"description":"Foo","entry":"20190820T203620Z","modified":"20190820T203620Z","start":"20190820T203620Z","status":"pending","tags":["Tag","Baz"],"uuid":"6cab88f0-ac12-4a87-995a-0e7d39810c05"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Foo", "Tag", "Baz"])

    def test_hook_should_process_modify_project(self):
        """on-modify hook should process 'task modify' for changing project"""
        self.t("start Foo dummy")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","project":"dummy","start":"20190820T203842Z","status":"pending","uuid":"d95dc7a0-6189-4692-b58a-4ab60d539c8d"}
{"description":"Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","project":"test","start":"20190820T203842Z","status":"pending","uuid":"d95dc7a0-6189-4692-b58a-4ab60d539c8d"}
""")

        self.assertEqual(b'', err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Foo", "test"])

    def test_hook_should_process_prepend(self):
        """on-modify hook should process 'task prepend'"""
        self.t("start 10min ago Foo")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","start":"20190820T203842Z","status":"pending","uuid":"02bc8839-b304-49f9-ac1a-29ac4850583f"}
{"description":"Prefix Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","start":"20190820T203842Z","status":"pending","uuid":"02bc8839-b304-49f9-ac1a-29ac4850583f"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Prefix Foo"])

    def test_hook_should_process_start(self):
        """on-modify hook should process 'task start'"""
        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","status":"pending","uuid":"16af44c5-57d2-43bf-97ed-cf2e541d927f"}
{"description":"Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","start":"20190820T203842Z","status":"pending","uuid":"16af44c5-57d2-43bf-97ed-cf2e541d927f"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertOpenInterval(j[0], expectedTags=["Foo"])

    def test_hook_should_process_stop(self):
        """on-modify hook should process 'task stop'"""
        self.t("start 10min ago Foo")

        out, err = self.process.communicate(input=b"""\
{"description":"Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","start":"20190820T203842Z","status":"pending","uuid":"13f83e99-f6a2-4857-9e00-bdeede064772"}
{"description":"Foo","entry":"20190820T203842Z","modified":"20190820T203842Z","status":"pending","uuid":"13f83e99-f6a2-4857-9e00-bdeede064772"}
""")

        self.assertEqual(bytes(b''), err)

        j = self.t.export()
        self.assertEqual(len(j), 1)
        self.assertClosedInterval(j[0], expectedTags=["Foo"])


if __name__ == "__main__":
    from simpletap import TAPTestRunner
    unittest.main(testRunner=TAPTestRunner())
