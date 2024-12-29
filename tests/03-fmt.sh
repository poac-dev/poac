#!/bin/sh

WHEREAMI=$(dirname "$(realpath "$0")")
export CABIN_TERM_COLOR='never'

test_description='Check if the cabin binary exists'

. $WHEREAMI/sharness.sh

command -v clang-format >/dev/null && test_set_prereq CLANG_FORMAT

if ! test_have_prereq CLANG_FORMAT; then
    test_expect_success 'cabin fmt without clang-format' '
        test_when_finished "rm -rf pkg" &&
        "$WHEREAMI"/../build/cabin new pkg &&
        cd pkg &&
        "$WHEREAMI"/../build/cabin fmt 2>actual &&
        cat >expected <<-EOF &&
Error: fmt command requires clang-format; try installing it by:
  apt/brew install clang-format
EOF
        test_cmp expected actual
    '

    skip_all='skipping fmt tests, clang-format not available'
    test_done
fi

test_expect_success 'cabin fmt' '
    test_when_finished "rm -rf pkg" &&
    "$WHEREAMI"/../build/cabin new pkg &&
    cd pkg &&
    (
        md5sum src/main.cc >before &&
        "$WHEREAMI"/../build/cabin fmt 2>actual &&
        md5sum src/main.cc >after &&
        test_must_fail test_cmp before after &&
        cat >expected <<-EOF &&
  Formatting pkg
EOF
        test_cmp expected actual
    )
'

test_expect_success 'cabin fmt no targets' '
    test_when_finished "rm -rf pkg" &&
    "$WHEREAMI"/../build/cabin new pkg &&
    cd pkg &&
    (
        rm src/main.cc &&
        "$WHEREAMI"/../build/cabin fmt 2>actual &&
        cat >expected <<-EOF &&
Warning: no files to format
EOF
        test_cmp expected actual
    )
'

test_done
