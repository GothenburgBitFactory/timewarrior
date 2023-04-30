# fish completion for timewarrior
# https://timewarrior.net/
# put this file in ~/.config/fish/completions/

function __fish_timew_get_commands
    timew help | sed -e '/^Usage:/d'  -e '/^Additional help:/Q' \
        -e 's/timew \[*\([a-z]\+\)\]*.*/\1/g;tx;d;:x' | string trim
end

function __fish_timew_get_tags
    timew tags | tail -n+4 | cut -d'-' -f1
end

function __fish_timew_get_ids
    timew summary :ids | sed -e 's/.*@\([0-9]\+\).*/@\1/g;tx;d;:x'
end

function __fish_timew_get_reports
    timew extensions | sed -e 's/^\(.*\).*Active/\1/g;tx;d;:x'
end

# Interval:
#        [from] <date>
#        [from] <date> to/- <date>
#        [from] <date> for <duration>
#        <duration> before/after <date>
#        <duration> ago
#        [for] <duration>
# timew show | sed -n '/\s\s.*:/p'

set -l commands (__fish_timew_get_commands)
set -l reports (__fish_timew_get_reports)
set -l ids (__fish_timew_get_ids)
set -l tags (__fish_timew_get_tags)
set -l intervals ""
set -l durations ""
set -l dates ""
set -l start_end "start end"


complete -c timew -f

complete -c timew -l version -d 'Print a short version string and exit'

set -l commands_with_description "
cancel\t'Cancel time tracking'
diagnostics\t'Show diagnostic information'
extensions\t'List available extensions'
show\t 'Display configuration'
undo\t'Revert Timewarrior commands'
annotate\t'Add an annotation to intervals'
config\t'Get and set Timewarrior configuration'
continue\t'Resume tracking of existing interval'
day\t'Display chart report'
delete\t'Delete intervals'
export\t'Export tracked time in JSON'
gaps\t'Display time tracking gaps'
get\t'Display DOM values'
help\t'Display help'
week\t'Display chart report'
join\t'Join intervals'
lengthen\t'Lengthen intervals'
modify\t'Change start or end date of an interval'
month\t'Display chart report'
move\t'Change interval start-time'
shorten\t'Shorten intervals'
split\t'Split intervals'
start\t'Start time tracking'
stop\t'Stop time tracking'
summary\t'Display a time-tracking summary'
tag\t'Add tags to intervals'
tags\t'Display a list of tags'
track\t'Add intervals to the database'
untag\t'Remove tags from intervals'
week\t'Display chart report'
"

# Base Commands 
complete -c timew -n "not __fish_seen_subcommand_from $commands" \
  -a "$commands_with_description" \
  -d "Timewarrior command"


complete -c timew -n "not __fish_seen_subcommand_from $commands" \
  -a "$reports" \
  -d "Report"


# Subcomands

complete -c timew -n "__fish_seen_subcommand_from annotate" \
  -a "$ids"
  # @<id> [@<id> ...] <annotation>

complete -c timew -n "__fish_seen_subcommand_from config" \
  -a ""
#[<name> [<value> | '']]

complete -c timew -n "__fish_seen_subcommand_from continue" \
  -a "$ids"
#[@<id>] [<date>|<interval>]

complete -c timew -n "__fish_seen_subcommand_from day" \
  -a "$tags"
#[<interval>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from delete" \
  -a "$ids"
#@<id> [@<id> ...]

complete -c timew -n "__fish_seen_subcommand_from export" \
  -a "$tags"
#[<interval>] [<tag> ...]
#
complete -c timew -n "__fish_seen_subcommand_from gaps" \
  -a "$tags"
# [<interval>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from get" \
  -a ""
# <DOM> [<DOM> ...]

complete -c timew -n "__fish_seen_subcommand_from join" \
  -a "$ids"
# @<id> @<id>

complete -c timew -n "__fish_seen_subcommand_from lengthen" \
  -a "$ids"
# @<id> [@<id> ...] <duration>

complete -c timew -n "__fish_seen_subcommand_from modify && not __fish_seen_subcommand_from $start_end" \
  -a "start end"
# (start|end) @<id> <date>

complete -c timew -n "__fish_seen_subcommand_from modify && __fish_seen_subcommand_from $start_end" \
  -a "$ids"
# (start|end) @<id> <date>


complete -c timew -n "__fish_seen_subcommand_from month" \
  -a "$tags"
# [<interval>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from move" \
  -a "$ids"
# @<id> <date>

complete -c timew -n "__fish_seen_subcommand_from $reports" \
  -a "$tags $interval"
# <report> [<interval>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from shorten" \
  -a "$ids"
# @<id> [@<id> ...] <duration>

complete -c timew -n "__fish_seen_subcommand_from split" \
  -a "$ids"
# @<id> [@<id> ...]

complete -c timew -n "__fish_seen_subcommand_from start && not __fish_seen_subcommand_from modify" \
  -a "$tags $dates"
# [<date>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from stop && not __fish_seen_subcommand_from modify" \
  -a "$tags"
# [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from summary" \
  -a "$tags $intervals"
# [<interval>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from tag" \
  -a "$ids $tags"
# @<id> [@<id> ...] <tag> [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from tags" \
  -a "$tags $intervals"
# [<interval>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from track" \
  -a "$tags $intervals"
# <interval> [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from untag" \
  -a "$ids $tags"
# @<id> [@<id> ...] <tag> [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from week" \
  -a "$tags $intervals"
# [<interval>] [<tag> ...]

complete -c timew -n "__fish_seen_subcommand_from help" \
  -a "$commands dates dom durations hints ranges" \
  -d "Show help"

 

