#!/usr/bin/env bash

if ! command -v poac >/dev/null 2>&1; then
    return
fi

__poac() {
  local words=${COMP_WORDS}
  local cword=${COMP_CWORD}
  local cur=${COMP_WORDS[COMP_CWORD]}
  local prev=${COMP_WORDS[COMP_CWORD-1]}

  case "${prev}" in
    uninstall) COMPREPLY=$(ls ./deps) ;;
    *) COMPREPLY=($__poac_commands) ;;
  esac

  return 0
}

# complete is a bash builtin, but recent versions of ZSH come with a function
# called bashcompinit that will create a complete in ZSH. If the user is in
# ZSH, load and run bashcompinit before calling the complete function.
if [[ -n ${ZSH_VERSION-} ]]; then
  autoload -U +X bashcompinit && bashcompinit
  autoload -U +X compinit && compinit
fi

complete -F __poac poac

__poac_commands='cache
doc
init
install
login
new
publish
root
run
search
uninstall
update'
