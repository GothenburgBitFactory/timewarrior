#!/usr/bin/env python
###############################################################################
#
# Copyright 2015 - 2016, Paul Beckingham, Federico Hernandez.
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
import json
import datetime


def format_seconds(seconds):
    """Convert seconds to a formatted string

    Convert seconds: 3661
    To formatted:    1:01:01
    """
    hours = int(seconds / 3600)
    minutes = int(seconds % 3600) / 60
    seconds = seconds % 60
    return '%4d:%02d:%02d' % (hours, minutes, seconds)


DATEFORMAT = '%Y%m%dT%H%M%SZ'

# Extract the configuration settings.
header = 1
configuration = dict()
body = ''
for line in sys.stdin:
    if header:
        if line == '\n':
            header = 0
        else:
            fields = line.strip().split(': ', 2)
            if len(fields) == 2:
                configuration[fields[0]] = fields[1]
            else:
                configuration[fields[0]] = ''
    else:
        body += line

# Sum the second tracked by tag.
totals = dict()
untagged = None
j = json.loads(body)
for object in j:
    start = datetime.datetime.strptime(object['start'], DATEFORMAT)

    if 'end' in object:
        end = datetime.datetime.strptime(object['end'], DATEFORMAT)
    else:
        end = datetime.datetime.utcnow()

    tracked = end - start

    if 'tags' not in object or object['tags'] == []:
        if untagged is None:
            untagged = tracked
        else:
            untagged += tracked
    else:
        for tag in object['tags']:
            if tag in totals:
                totals[tag] += tracked
            else:
                totals[tag] = tracked

# Determine largest tag width.
max_width = len('Total')
for tag in totals:
    if len(tag) > max_width:
        max_width = len(tag)

if 'temp.report.start' not in configuration:
    print('There is no data in the database')
    exit()

start = datetime.datetime.strptime(configuration['temp.report.start'], DATEFORMAT)
end = datetime.datetime.strptime(configuration['temp.report.end'], DATEFORMAT)

if max_width > 0:
    # Compose report header.
    print('\nTotal by Tag, for %s - %s\n' % (start, end))

    # Compose table header.
    if configuration['color'] == 'on':
        print('[4m%-*s[0m [4m%10s[0m' % (max_width, 'Tag', 'Total'))
    else:
        print('%-*s %10s' % (max_width, 'Tag', 'Total'))
        print('{} {}'.format('-' * max_width, '----------'))

    # Compose table rows.
    grand_total = 0
    for tag in sorted(totals):
        formatted = format_seconds(totals[tag].seconds)
        grand_total += totals[tag].seconds
        print('%-*s %10s' % (max_width, tag, formatted))

    if untagged is not None:
        formatted = format_seconds(untagged.seconds)
        grand_total += untagged.seconds
        print('%-*s %10s' % (max_width, '', formatted))

    # Compose total.
    if configuration['color'] == 'on':
        print('{} {}'.format(' ' * max_width, '[4m          [0m'))
    else:
        print('{} {}'.format(' ' * max_width, '----------'))

    print('%-*s %10s' % (max_width, 'Total', format_seconds(grand_total)))

else:
    print('No data in the range %s - %s' % (start, end))
