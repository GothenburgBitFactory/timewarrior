#!/bin/sh
BASEDIR=$(dirname "$0")
if ldd ${BASEDIR}/atomic | grep -q 'libfiu' ; then
    exec fiu-run -x ${BASEDIR}/atomic
else
    exec ${BASEDIR}/atomic
fi
