#!/bin/bash

ROOT_DIR=$PWD
BASE_OPT="-std=c++1z -I${ROOT_DIR}/include -lboost_unit_test_framework -fprofile-arcs -ftest-coverage"

pushd ./tests/poac


pushd ./core/builder
g++ ${BASE_OPT} -lboost_filesystem -o standard-test standard.cpp && ./standard-test && rm -rf ./standard-test
popd

pushd ./core/resolver
g++ ${BASE_OPT} -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lboost_filesystem -lssl -lcrypto -ldl -lyaml-cpp -DPOAC_VERSION=\"0.2.1\" -DPOAC_PROJECT_ROOT=\"${ROOT_DIR}\" -o resolve-test resolve.cpp
./resolve-test
g++ ${BASE_OPT} -o sat-test sat.cpp
./sat-test
popd

pushd ./core
g++ ${BASE_OPT} -o exception-test except.cpp
./exception-test
g++ ${BASE_OPT} -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lboost_filesystem -lssl -lcrypto -ldl -lyaml-cpp -DPOAC_VERSION=\"0.2.1\" -DPOAC_PROJECT_ROOT=\"${ROOT_DIR}\" -o infer-test infer.cpp
./infer-test
g++ ${BASE_OPT} -lboost_filesystem -lyaml-cpp -o name-test name.cpp
./name-test
popd

pushd ./io
g++ ${BASE_OPT} -o cli-test cli.cpp
./cli-test
g++ ${BASE_OPT} -lboost_filesystem -lyaml-cpp -o yaml-test yaml.cpp
./yaml-test
popd

pushd ./opts
g++ ${BASE_OPT} -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lboost_filesystem -lssl -lcrypto -ldl -lyaml-cpp -DPOAC_VERSION=\"0.2.1\" -DPOAC_PROJECT_ROOT=\"${ROOT_DIR}\" -o publish-test publish.cpp && ./publish-test
popd

pushd ./util
g++ ${BASE_OPT} -o argparse-test argparse.cpp && ./argparse-test
g++ ${BASE_OPT} -o misc-test misc.cpp && ./misc-test
g++ ${BASE_OPT} -o pretty-test pretty.cpp && ./pretty-test
  pushd ./semver
    g++ ${BASE_OPT} -o lexer-test lexer.cpp && ./lexer-test
  popd
g++ ${BASE_OPT} -o semver-test semver.cpp && ./semver-test
g++ ${BASE_OPT} -o shell-test shell.cpp && ./shell-test
g++ ${BASE_OPT} -o types-test types.cpp && ./types-test
popd


popd

# Run gcov and upload report to coveralls
coveralls --exclude build --gcov-options '\-lp' -t ${COVERALLS_TOKEN}
bash <(curl -s https://codecov.io/bash)
