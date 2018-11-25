#!/bin/bash

YESTERDAY=$(date -d "yesterday" "+%Y%m%d")

export TIMEWARRIORDB=/home/lauft/Projects/timew/timewarriordb
rm -f ${TIMEWARRIORDB}/data/*.data
:> ${TIMEWARRIORDB}/timewarrior.cfg

timew start ${YESTERDAY}T124452Z homework
timew month
timew week
timew day

