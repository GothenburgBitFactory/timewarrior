#!/usr/bin/env python3

###############################################################################
#
# Copyright 2016 - 2020, Thomas Lauf, Paul Beckingham, Federico Hernandez.
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

import datetime
import json

import sys
from dateutil import tz

DATEFORMAT = "%Y%m%dT%H%M%SZ"


def format_seconds(seconds):
    """Convert seconds to a formatted string

    Convert seconds: 3661
    To formatted: "   1:01:01"
    """
    hours = seconds // 3600
    minutes = seconds % 3600 // 60
    seconds = seconds % 60
    return "{:4d}:{:02d}:{:02d}".format(hours, minutes, seconds)


def calculate_totals(input_stream):
    from_zone = tz.tzutc()
    to_zone = tz.tzlocal()

    # Extract the configuration settings.
    header = 1
    configuration = dict()
    body = ""
    for line in input_stream:
        if header:
            if line == "\n":
                header = 0
            else:
                fields = line.strip().split(": ", 2)
                if len(fields) == 2:
                    configuration[fields[0]] = fields[1]
                else:
                    configuration[fields[0]] = ""
        else:
            body += line

    # Sum the seconds tracked by tag.
    totals = dict()
    untagged = None
    j = json.loads(body)
    for object in j:
        start = datetime.datetime.strptime(object["start"], DATEFORMAT)

        if "end" in object:
            end = datetime.datetime.strptime(object["end"], DATEFORMAT)
        else:
            end = datetime.datetime.utcnow()

        tracked = end - start

        if "tags" not in object or object["tags"] == []:
            if untagged is None:
                untagged = tracked
            else:
                untagged += tracked
        else:
            for tag in object["tags"]:
                if tag in totals:
                    totals[tag] += tracked
                else:
                    totals[tag] = tracked

    # Determine largest tag width.
    max_width = len("Total")
    for tag in totals:
        if len(tag) > max_width:
            max_width = len(tag)

    if "temp.report.start" not in configuration:
        return ["There is no data in the database"]

    start_utc = datetime.datetime.strptime(configuration["temp.report.start"], DATEFORMAT)
    start_utc = start_utc.replace(tzinfo=from_zone)
    start = start_utc.astimezone(to_zone)

    if "temp.report.end" in configuration:
        end_utc = datetime.datetime.strptime(configuration["temp.report.end"], DATEFORMAT)
        end_utc = end_utc.replace(tzinfo=from_zone)
        end = end_utc.astimezone(to_zone)
    else:
        end = datetime.datetime.now()

    if len(totals) == 0 and untagged is None:
        return ["No data in the range {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}".format(start, end)]

    # Compose report header.
    output = [
        "",
        "Total by Tag, for {:%Y-%m-%d %H:%M:%S} - {:%Y-%m-%d %H:%M:%S}".format(start, end),
        ""
    ]

    # Compose table header.
    if configuration["color"] == "on":
        output.append("[4m{:{width}}[0m [4m{:>10}[0m".format("Tag", "Total", width=max_width))
    else:
        output.append("{:{width}} {:>10}".format("Tag", "Total", width=max_width))
        output.append("{} {}".format("-" * max_width, "----------"))

    # Compose table rows.
    grand_total = 0
    for tag in sorted(totals):
        seconds = int(totals[tag].total_seconds())
        formatted = format_seconds(seconds)
        grand_total += seconds
        output.append("{:{width}} {:10}".format(tag, formatted, width=max_width))

    if untagged is not None:
        seconds = int(untagged.total_seconds())
        formatted = format_seconds(seconds)
        grand_total += seconds
        output.append("{:{width}} {:10}".format("", formatted, width=max_width))

    # Compose total.
    if configuration["color"] == "on":
        output.append("{} {}".format(" " * max_width, "[4m          [0m"))
    else:
        output.append("{} {}".format(" " * max_width, "----------"))

    output.append("{:{width}} {:10}".format("Total", format_seconds(grand_total), width=max_width))
    output.append("")

    return output


if __name__ == "__main__":
    for line in calculate_totals(sys.stdin):
        print(line)
