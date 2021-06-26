# Timewarrior Extensions

## on-start.timewarrior & on-modify.timewarrior
Taskwarrior hook scripts that integrate it with Timewarrior.

## test-hooks.sh
A script to test the Taskwarrior hooks. 
It uses a temporary environment under /tmp to ensure reproducibility and not tamper with your personal data.

## debug.py
This is a debug extension that simply echoes back everything it sees in its input.
It's useful for debugging to see how the command line affects what an extension sees.
This will likely be removed before release.

## csv.py
This extension exports the data in CSV format, with a variable number of tag fields.

## totals.py
Sample extension report that shows totals by tag.

