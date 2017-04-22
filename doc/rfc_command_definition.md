# RFC: TimeWarrior command definition

TimeWarrior can be used like this:

    timew [--version | --help | <command> [<args>]]

### Arguments
* `command` is one of the commands listed below.
* `args` is an optional list of command arguments.

### Behavior
* `timew` prints the current time tracking status and exits.
* `timew --version` prints version information and exits.
* `timew --help` prints general help, references other help, and exits.
`timew --help` is converted internally to `timew help`.
* `timew <command> [<args>]` calls the specified `command` with the given arguments `args`.

## Command line elements
A basic view on the command line:

    timew <words>

### `Word`
A `word` is the basic element of the command line.
`Words` are separated by whitespace (except when quoted).

### `Command`
A `command` is first element on the command line.
The CLI identifies the command, and categorizes all words.
further interpretation of those words is delegated to teh command.
The commands parse the command arguments to execute their functionality.

### `Subcommand`
Analog to `commands`, `subcommands` are the first element of the command arguments.
`Subcommands` are used to specify command behavior.
For example

    timew tag list
    timew tag remove @3 foo bar

(PB: Probably need better examples)

### `Date`
A `date` specifies a certain point in time.
Examples are
* `1pm`
* `10:33`
* `2017-04-11T17:12:23`
* `...`

### `Duration`
A `duration` can be specified by
* `one day`, `two weeks`
* `today`, `yesterday`
* `...`

### `Range`
A `range` is a time interval.
It can be specified as the difference between two `dates` or as a combination of a `date` and a `duration`. E.g.
* `09:45 - 11:15`
* `two hours before 10:45` which means `8:45 - 10:45`
* `30mins until 2017-03-23T13:13` which means `2017-03-23T12:43 - 2017-03-23T13:13`
* `one hour ago` which means `-1hr - now`

### `Tag`
A `tag` is a lexical token that is at least one non-whitespace character, and
could be a quoted phrase.  It is treated as one word.  It is used to label an
`interval`.

### `Filter`
Commands can use `filters` to limit the list of intervals they operate on.
Filters can be
* `tags`
* `ranges`

### `Hints`
Hints are words that begin wіth the `:` character, for example `:quiet`.
A hint is a request that behavior be modified in a specific way.  The example of
the `:quiet` hint requests that no extraneous output is generated, making the
command more useful for scripting.

There is a supported set of hints (`timew help hints`), and commands will
respond to hints that make sense in context, and ignore those that do not.

It is not an error to specify unrelated (but supported) hints. Commands may
implicitly behave as though a hint were specified. For this reason hints are
so named, they are not necessarily obeyed, not necessarily relevant.

## Commands
Sorting the command line zoo.

### Basic commands
Basic commands provide the minimal TimewWarrior functionality.
All arguments are optional.
* `start`
* `stop`
* `cancel`

### Modifying commands
These commands modify existing intervals. They require an `id` to know on which interval(s) to operate on.
* `tag/untag`
* `delete`
* `shorten/lengthen/resize`
* `move`
* `fill`
* `split`
* `join`

### Listing commands
Listing and reporting on the database...
* `export`
* `summary`
* `chart (day, week, month)`
* `gaps`
* `tags`
* `report`

### Shortcut commands
Shortcut commands combine functionality of basic commands.
* `track` (combines start & stop)
* `continue` (combines tags & start)

### Administrative commands
Not typical user-level commands, but provide support and diagnostic features...
* `help`
* `show`
* `config`
* `get`
* `diagnostics`
* `extensions`

# Command definitions
## Command `start`
### Synopsis

    timew start [<date>] [<tags>]

### Arguments
* `date` is an optional argument. If no date is given, the current date is used.
* `tags` is an optional list of one or more tags.

### Behavior
* If there is no active time tracking, `start` creates a new open interval at the given date and with the given tags.
* In case of active time tracking, `start` closes the current open interval at the given date and creates a new open interval only if the given tag list differs from the tag list of the current open interval.

## Command `stop`
### Synopsis

    timew stop [<date>] [<tags>]

### Arguments
* `date` is an optional argument. If no date is given, the current date is used.
* `tags` is an optional list of one or more tags.

### Behavior
* If there is no active time tracking `stop` has no effect.
* In case of active time tracking, `stop` closes the current open interval.
* If a tag list is given, a new open interval is created with all tags which are part of the current interval's tag list but not part of the given tag list.
Tags which are part of the given tag list but not part of the current interval's tag list are ignored.

### Synopsis

## Command `cancel`
    timew cancel

### Arguments
* `cancel` has no arguments.

### Behavior
* If there is no active time tracking `cancel` has no effect.
* In case of active time tracking, `cancel` deletes the current open interval.

## Command `track`
### Synopsis

    timew track <range> [<tags>]

### Arguments
* `range` is a date range: `<start-date> - <stop-date>`.
* `tags` is an optional list of one or more tags.

### Behavior
`timew track <start-date> - <stop-date> <tags>` acts as a shortcut for

    timew start <start-date> <tags>
    timew stop <stop-date>

and thus behaves the same way.

## Command `continue`
### Synopsis

    timew continue [<id>]

### Arguments
* `id` is an optional argument. If no id is given, `@1` is used.

### Behavior
`timew continue <id>` acts as a shortcut for

    timew start <tag-list-of-interval-with-given-id>

and thus behaves the same way.

## Command `tag`
### Synopsis

    timew tag [remove] [<id>] <tags>
    timew tag list [<ids>]

### Arguments
* `remove` is an optional subcommand. If given, the specified tags are removed and not added.
* `id` is an optional argument. If no id is given, `@1` is used.
* `tags` is a list of one or more tags.
* `list` is an optional subcommand.
* `ids` is an optional list of one or more ids.

### Behavior
* Tags which are not in the tag list of the specified interval are ignored.
* If no id is given, `tag list` lists all available tags.
