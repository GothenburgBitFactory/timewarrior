#!/usr/bin/env bash
# This file only contains helper functions for making Timewarrior testing
# easier. The magic happens in bash_tap.sh sourced at the end of this file.
#
# "timew" is a bash function calling "TIMEWARRIORDB=dir /path/to/compiled/timew"
# Only local paths are searched, see bash_tap_tw.sh:find_timew_binary().
#
# "timewarrior.cfg" is a file set up in bash_tap_ti.sh:setup_cfg(), and can be
# appended to or changed as needed.
#
# Subject to the MIT License. See LICENSE file or http://opensource.org/licenses/MIT
# Copyright (c) 2015-2017 Wilhelm Schürmann

function setup_cfg {
    # Configuration
    for i in data/*.data timewarrior.cfg; do
       if [ -f "$i" ]; then
           rm "$i" 2>&1 >/dev/null
       fi
    done

    export TIMEWARRIORDB=.

    #echo 'foo'  >> timewarrior.cfg
}

function find_timew_binary {
    # $bashtap_org_pwd is set in bash_tap.sh. It is the directory the parent script is
    # run from. Check for the tiemw binary relative to that directory.
    # Do not use the system "tiemw" if no local one is found, error out instead.
    for t in "${bashtap_org_pwd}/timew" "${bashtap_org_pwd}/src/timew" "${bashtap_org_pwd}/../timew" "${bashtap_org_pwd}/../src/timew" "${bashtap_org_pwd}/../build/src/timew"; do
        if [ -f "$t" ] && [ -x "$t" ]; then
            t_abs=$(bashtap_get_absolute_path "$t")
            eval "function timew { ${t_abs} \"\$@\"; }"
            return 0
        fi
    done

    echo "# ERROR: Could not find timew binary!"

    # Needed for normal, i.e. "non-test" mode.
    eval "function timew { exit; }"

    # Set $line so we can have useful TAP output.
    line="bash_tap.sh:find_timew_binary()"

    return 1
}

function bashtap_setup {
    # This function is called by bash_tap.sh before running tests, or before
    # running the parent script normally.
    find_timew_binary
    setup_cfg
}


# Include the base script that does the actual work.
source bash_tap.sh
