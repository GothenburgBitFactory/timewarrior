#!/usr/bin/env python

import sys
import json

# Skip the configuration settings.
for line in sys.stdin:
    if line == '\n':
        break

# Extract the JSON.
doc = ''
for line in sys.stdin:
    doc += line

total_active_time = 0

j = json.loads(doc)
for object in j:
    line = '"%s",' % object['start']
    line += '"%s"' % (object['end'] if 'end' in object else '')

    if 'tags' in object:
        for tag in object['tags']:
            line += ',"%s"' % tag

    print(line)
