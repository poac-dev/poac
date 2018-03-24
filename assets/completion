#!/bin/bash

_poac() {
  local cur=${COMP_WORDS[COMP_CWORD]}
  local prev=${COMP_WORDS[COMP_CWORD-1]}
  local opts="$(find $(poac root)/libexec -maxdepth 1 -type f -name 'poac-*' | awk -F '/' '{print $NF}' | awk -F 'poac-' '{ print $2 }')"
  COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
}
complete -F _poac poac
