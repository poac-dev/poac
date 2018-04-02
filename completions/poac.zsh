#compdef poac
_poac () {
  local e=$(dirname ${funcsourcetrace[1]%:*})/poac
  [[ -f $e ]] && source $e
}
