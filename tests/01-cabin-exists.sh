#!/bin/sh

WHEREAMI=$(dirname "$(realpath "$0")")
export CABIN_TERM_COLOR='never'

test_description='Check if the cabin binary exists'

. $WHEREAMI/sharness.sh

test_expect_success 'The cabin binary exists' '
    test -x "$WHEREAMI"/../build/cabin
'

test_done
