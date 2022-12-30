#!/usr/bin/env python3

import sys

for line in sys.stdin:
    # skip configuration
    if line == "\n":
        break

for line in sys.stdin:
    print(line.strip())
