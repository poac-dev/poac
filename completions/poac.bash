#!/bin/bash

__poac () {
  local cur=${COMP_WORDS[COMP_CWORD]}
  local prev=${COMP_WORDS[COMP_CWORD-1]}
  local opts="$(find $(poac root)/libexec -maxdepth 1 -type f -name 'poac-*' | awk -F '/' '{print $NF}' | awk -F 'poac-' '{ print $2 }')"
  COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
}

# complete is a bash builtin, but recent versions of ZSH come with a function
# called bashcompinit that will create a complete in ZSH. If the user is in
# ZSH, load and run bashcompinit before calling the complete function.
if [[ -n ${ZSH_VERSION-} ]]; then
  autoload -U +X bashcompinit && bashcompinit
fi

complete -F __poac poac
