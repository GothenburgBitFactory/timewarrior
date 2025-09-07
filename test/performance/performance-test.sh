#!/bin/bash

set -euo pipefail

# Use the installed version of Timewarrior or the one in PATH
TIMEW_BIN="$( command -v timew )"

if [[ -z "${TIMEW_BIN}" || ! -x "${TIMEW_BIN}" ]]; then
    echo "Timewarrior not found in PATH"
    exit 1
fi

# Output directory - use first argument or default to /performance-test/output
OUTPUT_DIR="${1-/performance-test/output}"

# Create a directory for hyperfine JSON output
mkdir -p "${OUTPUT_DIR}"
rm -rf "${OUTPUT_DIR:?}"/*

# Run performance tests with hyperfine
# annotate
hyperfine --export-json "${OUTPUT_DIR}/timew-annotate.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} annotate @1 ANNOTATION"

# cancel
hyperfine --export-json "${OUTPUT_DIR}/timew-cancel.json" \
  --style basic \
  --prepare "${TIMEW_BIN} start 20250907T100000 TEST" \
  "${TIMEW_BIN} cancel"

# continue
hyperfine --export-json "${OUTPUT_DIR}/timew-continue.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} cancel; ${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} continue @1"

# day
hyperfine --export-json "${OUTPUT_DIR}/timew-day.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} day"

# delete
hyperfine --export-json "${OUTPUT_DIR}/timew-delete.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  "${TIMEW_BIN} delete @1"

# export
hyperfine --export-json "${OUTPUT_DIR}/timew-export.json" \
  --style basic \
  "${TIMEW_BIN} export"

# gaps
hyperfine --export-json "${OUTPUT_DIR}/timew-gaps.json" \
  --style basic \
  "${TIMEW_BIN} gaps"

# get
hyperfine --export-json "${OUTPUT_DIR}/timew-get.json" \
  --style basic \
  "${TIMEW_BIN} get dom.active"

# join
hyperfine --export-json "${OUTPUT_DIR}/timew-join.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T080000 - 20250907T090000 TEST; ${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} join @1 @2"

# lengthen
hyperfine --export-json "${OUTPUT_DIR}/timew-lengthen.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T080000 - 20250907T090000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} lengthen @1 30min"

# month
hyperfine --export-json "${OUTPUT_DIR}/timew-month.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} month"

# modify-end
hyperfine --export-json "${OUTPUT_DIR}/timew-modify-end.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T080000 - 20250907T090000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} modify end @1 20250907T100000"

# modify-start
hyperfine --export-json "${OUTPUT_DIR}/timew-modify-start.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} modify start @1 20250907T080000"

# move
hyperfine --export-json "${OUTPUT_DIR}/timew-move.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T080000 - 20250907T090000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} move @1 20250907T100000"

# resize
hyperfine --export-json "${OUTPUT_DIR}/timew-resize.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} resize @1 30min"

# shorten
hyperfine --export-json "${OUTPUT_DIR}/timew-shorten.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T120000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} shorten @1 30min"

# split
hyperfine --export-json "${OUTPUT_DIR}/timew-split.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T120000 TEST" \
  --conclude "${TIMEW_BIN} delete @1 @2" \
  "${TIMEW_BIN} split @1"

# start
hyperfine --export-json "${OUTPUT_DIR}/timew-start.json" \
  --style basic \
  --conclude "${TIMEW_BIN} cancel" \
  "${TIMEW_BIN} start 20250907T100000 TEST"

# stop
hyperfine --export-json "${OUTPUT_DIR}/timew-stop.json" \
  --style basic \
  --prepare "${TIMEW_BIN} start 20250907T100000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} stop 20250907T110000 TEST"

# summary
hyperfine --export-json "${OUTPUT_DIR}/timew-summary.json" \
  --style basic \
  "${TIMEW_BIN} summary"

# tag
hyperfine --export-json "${OUTPUT_DIR}/timew-tag.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} tag @1 TAG"

# tags
hyperfine --export-json "${OUTPUT_DIR}/timew-tags.json" \
  --style basic \
  "${TIMEW_BIN} tags"

# track
hyperfine --export-json "${OUTPUT_DIR}/timew-track.json" \
  --style basic \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST"

# undo
hyperfine --export-json "${OUTPUT_DIR}/timew-undo.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  "${TIMEW_BIN} undo"

# untag
hyperfine --export-json "${OUTPUT_DIR}/timew-untag.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} untag @1 TEST"

# week
hyperfine --export-json "${OUTPUT_DIR}/timew-week.json" \
  --style basic \
  --prepare "${TIMEW_BIN} track 20250907T100000 - 20250907T110000 TEST" \
  --conclude "${TIMEW_BIN} delete @1" \
  "${TIMEW_BIN} week"

echo "Performance tests completed. Results are in ${OUTPUT_DIR}"
