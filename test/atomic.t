#!/bin/sh
BASEDIR=$(dirname "$0")

if [ "$(uname -s)" = "Darwin" ] ; then
  DLL_TOOL="otool -L"
else
  DLL_TOOL="ldd"
fi

if ${DLL_TOOL} ${BASEDIR}/atomic | grep -q 'libfiu' ; then
    exec fiu-run -x ${BASEDIR}/atomic
else
    exec ${BASEDIR}/atomic
fi
