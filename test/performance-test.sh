#!/bin/bash

set -e

# Due to system integrity protection, macOS requires us to use a copy of date
# if we want it to work with faketimea.
DATE=$(command -v date)
if [ $(uname -s) = "Darwin" ]; then
    TEMP_DATE=$(mktemp)
    cp $DATE $TEMP_DATE
    chmod +x $TEMP_DATE
    DATE=$TEMP_DATE
    function cleanup {
        rm -fr $DATE
    }
    trap cleanup EXIT
fi

THREE_HOURS_BEFORE=$( faketime '3 hours ago' ${DATE} "+%Y%m%dT%H0000" )
TWO_HOURS_BEFORE=$( faketime '2 hours ago' ${DATE} "+%Y%m%dT%H0000" )
ONE_HOUR_BEFORE=$( faketime '1 hours ago' ${DATE} "+%Y%m%dT%H0000" )

TIMEW_BIN="${BASH_SOURCE[0]%/*}/../src/timew"

[[ -x "${TIMEW_BIN}" ]] || exit 1

function test_performance_annotate()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} annotate @1 ANNOTATION >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_cancel()
{
  # setup
  ${TIMEW_BIN} start "${TWO_HOURS_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} cancel >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
}

function test_performance_continue()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} continue @1 >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} cancel >/dev/null
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_day()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} day >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_delete()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} delete @1 >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
}

function test_performance_export()
{
  # test
  ( ( time -p (
      ${TIMEW_BIN} export >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
}

function test_performance_gaps()
{
  # test
  ( ( time -p (
      ${TIMEW_BIN} gaps >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
}

function test_performance_get()
{
  # test
  ( ( time -p (
      ${TIMEW_BIN} get dom.active >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
}

function test_performance_join()
{
  # setup
  ${TIMEW_BIN} track "${THREE_HOURS_BEFORE}" - "${TWO_HOURS_BEFORE}" TEST >/dev/null
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} join @1 @2  >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_lengthen()
{
  # setup
  ${TIMEW_BIN} track "${THREE_HOURS_BEFORE}" - "${TWO_HOURS_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} lengthen @1 30min >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_month()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} month >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_modify-end()
{
  # setup
  ${TIMEW_BIN} track "${THREE_HOURS_BEFORE}" - "${TWO_HOURS_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} modify end @1 "${ONE_HOUR_BEFORE}" >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_modify-start()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} modify start @1 "${THREE_HOURS_BEFORE}" >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_move()
{
  # setup
  ${TIMEW_BIN} track "${THREE_HOURS_BEFORE}" - "${TWO_HOURS_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} move @1 "${TWO_HOURS_BEFORE}" >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_resize()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} resize @1 30min >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_shorten()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} shorten @1 30min >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_split()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} split @1 >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 @2 >/dev/null
}

function test_performance_start()
{
  # test
  ( ( time -p (
      ${TIMEW_BIN} start "${TWO_HOURS_BEFORE}" TEST >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} cancel >/dev/null
}

function test_performance_stop()
{
  # setup
  ${TIMEW_BIN} start "${TWO_HOURS_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} stop "${ONE_HOUR_BEFORE}" TEST >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_summary()
{
  # test
  ( ( time -p (
      ${TIMEW_BIN} summary >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
}

function test_performance_tag()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} tag @1 TAG >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_tags()
{
  # test
  ( ( time -p (
      ${TIMEW_BIN} tags >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
}

function test_performance_track()
{
  # test
  ( ( time -p (
      ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_undo()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} undo >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
}

function test_performance_untag()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} untag @1 TEST >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

function test_performance_week()
{
  # setup
  ${TIMEW_BIN} track "${TWO_HOURS_BEFORE}" - "${ONE_HOUR_BEFORE}" TEST >/dev/null
  # test
  ( ( time -p (
      ${TIMEW_BIN} week >/dev/null
  ) 2>&1 >/dev/null ) | awk '{a[NR]=$2}; END {for(i=1;i<=3;i++){printf "%s\t",a[i]}}')
  # cleanup
  ${TIMEW_BIN} delete @1 >/dev/null
}

OUTPUT_DIR="${1-/tmp/timew-performance}"

export TIMEWARRIORDB=/tmp/timewarriordb
mkdir -p ${TIMEWARRIORDB}/data
rm -f ${TIMEWARRIORDB}/data/*.data
:> ${TIMEWARRIORDB}/timewarrior.cfg

mkdir -p "${OUTPUT_DIR}"
rm -rf "${OUTPUT_DIR:?}"/*

TIMEW_COMMANDS="annotate cancel continue day delete export gaps get join lengthen modify-end modify-start month move resize shorten split start stop summary tag tags track undo untag week"

# Write headers
for timew_cmd in ${TIMEW_COMMANDS} ; do
  echo -e "#Performance ${timew_cmd}" > "${OUTPUT_DIR}/timew-${timew_cmd}-performance.log"
  echo -e "#TAG\tENTRIES\tREAL\tUSER\tSYS" >> "${OUTPUT_DIR}/timew-${timew_cmd}-performance.log"
done

for step in $( seq 0 20 ) ; do
  YEAR_MONTH=$( faketime "${step} months ago" ${DATE} "+%Y-%m" )

  if [[ "${step}" -gt 0 ]] ; then
    year=${YEAR_MONTH%-*}
    month=${YEAR_MONTH#*-}
    # Increase database by 100 entries
    for day in $(seq -w 1 25) ; do
      {
        echo "inc ${year}${month}${day}T040000Z - ${year}${month}${day}T050000Z # FOO"
        echo "inc ${year}${month}${day}T073000Z - ${year}${month}${day}T113000Z # BAR"
        echo "inc ${year}${month}${day}T163000Z - ${year}${month}${day}T183000Z # BAZ"
        echo "inc ${year}${month}${day}T193000Z - ${year}${month}${day}T203000Z # FOO BAR"
       } >> "${TIMEWARRIORDB}/data/${year}-${month}.data"
    done
    ENTRIES=$( wc -l /tmp/timewarriordb/data/????-??.data | awk '{a=$1} ; END {printf "%s",a}' )
  fi

  echo -en "Testing with ${ENTRIES-0} entries...\r"

  # Run performance tests
  for timew_cmd in ${TIMEW_COMMANDS} ; do
    {
      echo -en "${YEAR_MONTH}\t${ENTRIES-0}\t"
      "test_performance_${timew_cmd}"
      echo ""
    }  >> "${OUTPUT_DIR}/timew-${timew_cmd}-performance.log"
  done
done
