#!/bin/bash

ROOT_DIR=$PWD
BASE_OPT="-std=c++1z -I${ROOT_DIR}/include -I/usr/local/include -L/usr/local/lib -lboost_unit_test_framework -fprofile-arcs -ftest-coverage"
REQUIRE_OPENSSL="-I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lssl -lcrypto -ldl"
REQUIRE_POAC_VARIABLES="-DPOAC_VERSION=\"0.2.1\""

execute() {
  echo "Executing: $@"
  "$@"
  echo
}

main() {
  pushd ./tests/poac

  pushd ./core
    pushd ./builder
      execute g++ ${BASE_OPT} -lboost_filesystem -o standard-test standard.cpp && { ./standard-test; rm -f ./standard-test; }
    popd
    pushd ./resolver
      execute g++ ${BASE_OPT} ${REQUIRE_OPENSSL} ${REQUIRE_POAC_VARIABLES} -lboost_filesystem -o resolve-test resolve.cpp && { ./resolve-test; rm -f ./resolve-test; }
      execute g++ ${BASE_OPT} -o sat-test sat.cpp && { ./sat-test; rm -f ./sat-test; }
    popd
    execute g++ ${BASE_OPT} ${REQUIRE_OPENSSL} ${REQUIRE_POAC_VARIABLES} -lboost_filesystem -lgit2 -o cli-test cli.cpp && { ./cli-test; rm -f ./cli-test; }
    execute g++ ${BASE_OPT} -o exception-test except.cpp && { ./exception-test; rm -f ./exception-test; }
    execute g++ ${BASE_OPT} -lboost_filesystem -o name-test name.cpp && { ./name-test; rm -f ./name-test; }
    execute g++ ${BASE_OPT} -lgit2 -o project-test project.cpp && { ./project-test; rm -f ./project-test; }
  popd
  pushd ./io
    execute g++ ${BASE_OPT} -lboost_filesystem -o config-test config.cpp && { ./config-test; rm -f ./config-test; }
    execute g++ ${BASE_OPT} -lboost_filesystem -o path-test path.cpp && { ./path-test; rm -f ./path-test; }
    execute g++ ${BASE_OPT} -o term-test term.cpp && { ./term-test; rm -f ./term-test; }
  popd
  pushd ./util
    execute g++ ${BASE_OPT} -o argparse-test argparse.cpp && { ./argparse-test; rm -f ./argparse-test; }
    execute g++ ${BASE_OPT} -o misc-test misc.cpp && { ./misc-test; rm -f ./misc-test; }
    execute g++ ${BASE_OPT} -o pretty-test pretty.cpp && { ./pretty-test; rm -f ./pretty-test; }
    pushd ./semver
      pushd ./parser
        execute g++ ${BASE_OPT} -o lexer-test lexer.cpp && { ./lexer-test; rm -f ./lexer-test; }
        execute g++ ${BASE_OPT} -o parser-test parser.cpp && { ./parser-test; rm -f ./parser-test; }
        execute g++ ${BASE_OPT} -o token-test token.cpp && { ./token-test; rm -f ./token-test; }
      popd
      execute g++ ${BASE_OPT} -o interval-test interval.cpp && { ./interval-test; rm -f ./interval-test; }
      execute g++ ${BASE_OPT} -o version-test version.cpp && { ./version-test; rm -f ./version-test; }
    popd
    execute g++ ${BASE_OPT} -o shell-test shell.cpp && { ./shell-test; rm -f ./shell-test; }
    execute g++ ${BASE_OPT} -o types-test types.cpp && { ./types-test; rm -f ./types-test; }
  popd

  popd

  pushd ./tests
  execute g++ ${BASE_OPT} ${REQUIRE_OPENSSL} ${REQUIRE_POAC_VARIABLES} -lboost_filesystem -lgit2 -o publish-test publish.cpp && { ./publish-test; rm -f ./publish-test; }
  popd
}

main
