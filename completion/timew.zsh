#compdef _timew timew

# zsh completion for timewarrior 1.7.1
# timewarrior_zsh_completion v0.1.0

_timew() {
    local ret=1

    _arguments -C \
        '1:command:->commands' \
        '*:ids_tags_hints:->ids_tags_hints' && ret=0

    case "$state" in
        commands)
            _describe 'commands' _commands
            ;;
        ids_tags_hints)
	    local arr vals

	    arr=()
	    # collected all tags from the database
	    vals=( "${(z)$(timew get dom.tracked.tags :all)}" )

            # add all of the tags to the output array
            for i in ${vals[@]}; do
		# strip out possible single double quotes from
		# multi-word tags
		arr+=( "$(echo $i | tr -d \" | tr -d \')" );
	    done

	    # collect interval ids and associated tags.

	    # iterate over the last 9 intervals
	    for n in {1..9}; do
		interval_tags=()

		# try and get the n-th interval tag count
		m_tags=$(timew get dom.tracked.${n}.tag.count 2>/dev/null)

		# if the interval wasn't found, exit loop
		if (( $? == 255 )); then
		    break
		fi

		# iterated over the m tags of interval n
		for m in {1..$m_tags}; do
		    tag=$(timew get dom.tracked.${n}.tag.${m} 2>/dev/null)
		    interval_tags+=("$tag")
		done
		arr+="@${n}:${interval_tags[@]}"
	    done

	    # append the predefined hints to the end
            arr+=( "${_hints[@]} ")

            _describe 'hints_tags_ids' arr
            ;;
    esac
}

_hints=(
    "\:adjust:Automatically correct overlaps"
    "\:all:All tracked time"
    "\:blank:Leaves tracked time out of a report"
    "\:color:Force color on, even if not connected to a TTY"
    "\:day:The 24 hours of the current day"
    "\:debug:Runs in debug mode, shows many runtime details"
    "\:fill:Expand time to fill surrounding available gap"
    "\:fortnight:This week and the one before"
    "\:ids:Displays interval ID numbers in the summary report"
    "\:lastmonth:Last month"
    "\:lastquarter:Last quarter"
    "\:lastweek:Last week"
    "\:lastyear:Last year"
    "\:month:This month"
    "\:nocolor:Force color off, even if connected to a TTY"
    "\:quarter:This quarter"
    "\:quiet:Turns off all feedback. For automation"
    "\:monday:Previous monday"
    "\:tuesday:Previous tuesday"
    "\:wednesday:Previous wednesday"
    "\:thursday:Previous thursday"
    "\:friday:Previous friday"
    "\:saturday:Previous saturday"
    "\:sunday:Previous sunday"
    "\:today:The 24 hours of the current day"
    "\:week:This week"
    "\:year:This year"
    "\:yes:Overrides confirmation by answering \'yes\' to the questions"
    "\:yesterday:The 24 hours of the previous day"
)

_commands=(
    'annotate:Add annotation to intervals'
    'cancel:Cancel time tracking'
    'config:Get and set Timewarrior configuration'
    'continue:Resume tracking of existing interval'
    'day:Display day chart'
    'delete:Delete intervals'
    'diagnostics:Show diagnostic information'
    'export:Export tracked time in JSON'
    'extensions:List available extensions'
    'get:Display DOM values'
    'help:Display help'
    'join:Join intervals'
    'lengthen:Lengthen intervals'
    'modify:Change start or end time of an interval'
    'month:Display month chart'
    'move:Change interval start-time'
    'report:Run an extension report'
    'resize:Set interval duration'
    'retag:Replace tags in intervals'
    'shorten:Shorten intervals'
    'show:Display configuration'
    'split:Split intervals'
    'start:Start time tracking'
    'stop:Stop time tracking'
    'summary:Display a time-tracking summary'
    'tag:Add tags to intervals'
    'tags:Display a list of tags'
    'track:Add intervals to the database'
    'undo:Undo Timewarrior commands'
    'untag:Remove tags from intervals'
    'week:Display week chart'
)
