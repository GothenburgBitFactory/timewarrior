#!/usr/bin/env bash
. bash_tap_ti.sh

echo 'define exclusions:'         >> timewarrior.cfg
echo '  monday    = <9:00 >18:00' >> timewarrior.cfg
echo '  tuesday   = <9:00 >18:00' >> timewarrior.cfg
echo '  wednesday = <9:00 >18:00' >> timewarrior.cfg
echo '  thursday  = <9:00 >18:00' >> timewarrior.cfg
echo '  friday    = <9:00 >18:00' >> timewarrior.cfg
echo '  saturday  = <9:00 >18:00' >> timewarrior.cfg
echo '  sunday    = <9:00 >18:00' >> timewarrior.cfg
echo                              >> timewarrior.cfg

# Deliberate start and end times that overlap (but do not enclose) the
# exclusions, to verify that the times are left intact.
timew track 8:31am - 6:29pm foo :quiet
timew export | grep 2900
timew export | grep 3100

