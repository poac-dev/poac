#!/bin/sh

WHEREAMI=$(dirname "$(realpath "$0")")
export CABIN_TERM_COLOR='never'

test_description='Test cabin new'

. $WHEREAMI/sharness.sh

test_expect_success 'cabin new hello_world' '
    test_when_finished "rm -rf hello_world" &&
    "$WHEREAMI"/../build/cabin new hello_world 2>actual &&
    (
        test -d hello_world &&
        cd hello_world &&
        test -d .git &&
        test -f .gitignore &&
        test -f cabin.toml &&
        test -d src &&
        test -f src/main.cc
    ) &&
    cat >expected <<-EOF &&
     Created binary (application) \`hello_world\` package
EOF
    test_cmp expected actual
'

test_done
