#!/bin/sh
BASEDIR=$(dirname "$0")

if [ "$(uname -s)" = "Darwin" ] ; then
  DLL_TOOL="otool -L"
else
  DLL_TOOL="ldd"
fi

if ${DLL_TOOL} ${BASEDIR}/AtomicFileTest | grep -q 'libfiu' ; then
    exec fiu-run -x ${BASEDIR}/AtomicFileTest
else
    exec ${BASEDIR}/AtomicFileTest
fi
