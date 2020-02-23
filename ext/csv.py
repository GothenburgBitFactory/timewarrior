#!/usr/bin/env python3

import json
import sys

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
    line = '"{}","{}"'.format(object['start'], (object['end'] if 'end' in object else ''))

    if 'tags' in object:
        for tag in object['tags']:
            line += ',"{}"'.format(tag)

    print(line)
