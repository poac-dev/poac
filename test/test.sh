#!/bin/bash
# print_summaries() {
#   for command; do
#     echo "$command"
#   done
# }

# print_summaries commands local global shell install uninstall rehash version versions which whence


# containsElement () {
#   local e="$2"
#   # shift
#   # echo $ma
#   for e; do [[ "$e" == "$1" ]] && return 0 || return 1; done
# }

# declare -a commands=('help' 'version' 'init' 'new' 'install')

# # # containsElement 'hep' "${commands[@]}"
# # # echo $?
# # case "${commands[@]}" in *"he"*) echo "found" ;; esac

# isin () { for e in "${@:2}"; do [[ "$e" = "$1" ]] && return '0' || return '1'; done; }
# ok=`isin 'help' "${commands[@]}" && echo $?`
# echo $ok

# touch test.txt
read -p "package name (${PWD##*/}): " name
echo $name
# declare -a array=('ok' 'ok2' 'ok3')



