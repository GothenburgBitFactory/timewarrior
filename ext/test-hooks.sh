#!/bin/sh
testdir=/tmp/timew-hook-test
rm -r $testdir
export TIMEWARRIORDB=$testdir/timew
export TASKDATA=$testdir/task
export TASKRC=$TASKDATA/taskrc
hooksdir=$TASKDATA/hooks
mkdir -p $hooksdir
cp ./*.timewarrior $hooksdir
( timew :quiet :yes && echo yes | task ) >/dev/null 2>&1

set -e
highlight() { echo "[4m$@[0m"; }
verbose=$(test "$1" = "v" && echo "true" || echo "false")
debug() { $verbose && $@ || true; }
if test "$1" = "q"
then task() { command task "$@" 2>/dev/null; }
else $verbose || task() { command task "$@" 3>&1 1>&2 2>&3 3>&- | (grep -v "override:" || true); }
fi
checkdom() {
	test "$(timew get dom.$1)" = "$2"
}

highlight A new task is started with correct time
task add test +t1 start:2020-04-04 entry:2020-04-03
debug task 1 info
checkdom active.start "2020-04-04T00:00:00"

highlight Modify start time
task 1 modify start:2020-04-05T12:00
debug timew
checkdom active.start "2020-04-05T12:00:00"

highlight Modify tags
task 1 modify +t2 project:testing
checkdom tag.count 4

checkcur() {
	checkdom tracked.count 1
	checkdom tag.count 4
	checkdom active.tag.count 4
	checkdom active.start "2020-04-05T12:00:00"
}

highlight Modify start time of second task
task add test2 +t3 entry:2020-04-03
task 2 modify start:2020-04-06
checkcur

highlight Modify tags of second now started task
task 2 modify +t3
debug timew
checkcur

highlight Stop second task
task 2 stop
checkcur

highlight Switch active task and don\'t override previous interval
task 1 stop
timew modify end @1 2020-04-07T00:00
#Automatically correct overlap, needs support in timew core
#task 2 modify start:today
! task 2 modify start:2020-04-05T00:00
task 2 modify start:2020-04-07
checkdom active.tag.count 2
