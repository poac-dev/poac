#!/bin/bash

ROOT_DIR=$PWD
BASE_OPT="-std=c++1z -I${ROOT_DIR}/include -lboost_unit_test_framework -fprofile-arcs -ftest-coverage"
REQUIRE_OPENSSL="-I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lssl -lcrypto -ldl"
REQUIRE_POAC_VARIABLES="-DPOAC_VERSION=\"0.2.1\" -DPOAC_PROJECT_ROOT=\"${ROOT_DIR}\""

execute () {
  echo "Executing: $@"
  "$@"
}

pushd ./tests/poac

pushd ./core
  pushd ./builder
    execute g++ ${BASE_OPT} -lboost_filesystem -o standard-test standard.cpp && { ./standard-test; rm -rf ./standard-test; }
  popd
  pushd ./resolver
    execute g++ ${BASE_OPT} ${REQUIRE_OPENSSL} ${REQUIRE_POAC_VARIABLES} -lboost_filesystem -lyaml-cpp -o resolve-test resolve.cpp && { ./resolve-test; rm -rf ./resolve-test; }
    execute g++ ${BASE_OPT} -o sat-test sat.cpp && { ./sat-test; rm -rf ./sat-test; }
  popd
  execute g++ ${BASE_OPT} ${REQUIRE_OPENSSL} ${REQUIRE_POAC_VARIABLES} -lboost_filesystem -lyaml-cpp -o cli-test cli.cpp && { ./cli-test; rm -rf ./cli-test; }
  execute g++ ${BASE_OPT} -o exception-test except.cpp && { ./exception-test; rm -rf ./exception-test; }
  execute g++ ${BASE_OPT} -lboost_filesystem -lyaml-cpp -o name-test name.cpp && { ./name-test; rm -rf ./name-test; }
popd
pushd ./io
  execute g++ ${BASE_OPT} -o term-test term.cpp && { ./term-test; rm -rf ./term-test; }
  execute g++ ${BASE_OPT} -lboost_filesystem -lyaml-cpp -o yaml-test yaml.cpp && { ./yaml-test; rm -rf ./yaml-test; }
popd
pushd ./opts
  execute g++ ${BASE_OPT} ${REQUIRE_OPENSSL} ${REQUIRE_POAC_VARIABLES} -lboost_filesystem -lyaml-cpp -o publish-test publish.cpp && { ./publish-test; rm -rf ./publish-test; }
popd
pushd ./util
  execute g++ ${BASE_OPT} -o argparse-test argparse.cpp && { ./argparse-test; rm -rf ./argparse-test; }
  execute g++ ${BASE_OPT} -o misc-test misc.cpp && { ./misc-test; rm -rf ./misc-test; }
  execute g++ ${BASE_OPT} -o pretty-test pretty.cpp && { ./pretty-test; rm -rf ./pretty-test; }
  pushd ./semver
    pushd ./parser
      execute g++ ${BASE_OPT} -o lexer-test lexer.cpp && { ./lexer-test; rm -rf ./lexer-test; }
      execute g++ ${BASE_OPT} -o parser-test parser.cpp && { ./parser-test; rm -rf ./parser-test; }
      execute g++ ${BASE_OPT} -o token-test token.cpp && { ./token-test; rm -rf ./token-test; }
    popd
    execute g++ ${BASE_OPT} -o interval-test interval.cpp && { ./interval-test; rm -rf ./interval-test; }
    execute g++ ${BASE_OPT} -o version-test version.cpp && { ./version-test; rm -rf ./version-test; }
  popd
  execute g++ ${BASE_OPT} -o shell-test shell.cpp && { ./shell-test; rm -rf ./shell-test; }
  execute g++ ${BASE_OPT} -o types-test types.cpp && { ./types-test; rm -rf ./types-test; }
popd

popd

# Run gcov and upload report to coveralls
coveralls --exclude build --gcov-options '\-lp' -t ${COVERALLS_TOKEN}
bash <(curl -s https://codecov.io/bash)
