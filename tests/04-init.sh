#!/bin/sh

WHEREAMI=$(dirname "$(realpath "$0")")
export CABIN_TERM_COLOR='never'

test_description='Test the init command'

. $WHEREAMI/sharness.sh

test_expect_success 'cabin init' '
    OUT=$(mktemp -d) &&
    test_when_finished "rm -rf $OUT" &&
    mkdir $OUT/pkg &&
    cd $OUT/pkg &&
    "$WHEREAMI"/../build/cabin init 2>actual &&
    cat >expected <<-EOF &&
     Created binary (application) \`pkg\` package
EOF
    test_cmp expected actual &&
    test -f cabin.toml
'

test_expect_success 'cabin init existing' '
    OUT=$(mktemp -d) &&
    test_when_finished "rm -rf $OUT" &&
    mkdir $OUT/pkg &&
    cd $OUT/pkg &&
    "$WHEREAMI"/../build/cabin init 2>actual &&
    cat >expected <<-EOF &&
     Created binary (application) \`pkg\` package
EOF
    test_cmp expected actual &&
    test -f cabin.toml
    test_must_fail "$WHEREAMI"/../build/cabin init 2>actual &&
    cat >expected <<-EOF &&
Error: cannot initialize an existing cabin package
Error: '\''cabin init'\'' failed with exit code \`1\`
EOF
    test_cmp expected actual &&
    test -f cabin.toml
'

test_done
