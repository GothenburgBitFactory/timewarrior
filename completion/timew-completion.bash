#!/usr/bin/env bash
#
# Bash completion for TimeWarrior
#
# Copyright (C) 2017 - 2019  Thomas Lauf
#
function __get_commands()
{
  echo "annotate cancel config continue day delete diagnostics export extensions gaps get help join lengthen modify month move report shorten show split start stop summary tag tags track undo untag week"
}

function __get_subcommands()
{
  case "${1}" in
    modify)
      echo "end start"
      ;;
    *)
      echo ""
      ;;
  esac
}

function __get_help_items()
{
  echo "$( __get_commands ) interval hints date duration dom"
}

function __get_options()
{
  echo "--help --verbose --version"
}

function __get_ids()
{
  seq -f "@%g" 1 "$( timew get dom.tracked.count )"
}

function __get_tags()
{
  timew tags | awk '{if(NR>3)print $1}'
}

function __get_extensions()
{
  timew extensions | awk '{if(NR>6)print $1}'
}

function __has_entered_id()
{
  for word in "${COMP_WORDS[@]}" ; do
    if [[ "${word}" =~ ^@ ]] ; then
      return 0
    fi
  done

  return 1
}

function __has_entered_subcommand()
{
  local subcommands=$( __get_subcommands "${1}" )

  for word in "${COMP_WORDS[@]}" ; do
    for cmd in ${subcommands} ; do
      if [[ "${word}" == "${cmd}" ]] ; then
        return 0
      fi
    done
  done

  return 1
}

function __has_entered_help_item()
{
  local items=$( __get_help_items )

  for word in "${COMP_WORDS[@]:2}" ; do
    for item in ${items} ; do
      if [[ "${word}" == "${item}" ]] ; then
        return 0
      fi
    done
  done

  return 1
}

function __is_entering_id()
{
  if [[ "${COMP_WORDS[COMP_CWORD]}" =~ @[0-9]* ]] ; then
    return 0
  else
    return 1
  fi
}

function _timew()
{
  local cur first
  COMPREPLY=()
  cur="${COMP_WORDS[COMP_CWORD]}"
  first="${COMP_WORDS[1]}"

  case "${first}" in
    cancel|config|diagnostics|day|extensions|get|month|show|undo|week)
      wordlist=""
      ;;
    annotate|continue|delete|join|lengthen|move|shorten|split)
      wordlist=$( __get_ids )
      ;;
    export|gaps|start|stop|summary|tags|track)
      wordlist=$( __get_tags )
      ;;
    modify)
      if __has_entered_subcommand "${first}" ; then
        wordlist=$( __get_ids )
      else
        wordlist=$( __get_subcommands "${first}" )
      fi
      ;;
    tag|untag)
      if __is_entering_id ; then
        wordlist=$( __get_ids )
      elif __has_entered_id ; then
        wordlist=$( __get_tags )
      else
        wordlist=$( __get_ids )
      fi
      ;;
    report)
      wordlist=$( __get_extensions )
      ;;
    help)
      if __has_entered_help_item ; then
        wordlist=""
      else
        wordlist=$( __get_help_items )
      fi
      ;;
    --*)
      wordlist=$( __get_options )
      ;;
    *)
      wordlist=$( __get_commands )
      ;;
  esac

  COMPREPLY=($( compgen -W "${wordlist}" -- "${cur}" ))
}

complete -F _timew timew
