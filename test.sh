#!/bin/bash

ROOT_DIR=$PWD
BASE_OPT="-std=c++1z -I${ROOT_DIR}/include -lboost_unit_test_framework -fprofile-arcs -ftest-coverage"

pushd ./test/poac


pushd ./core/deper
g++ ${BASE_OPT} -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lboost_filesystem -lssl -lcrypto -ldl -lyaml-cpp -DPOAC_VERSION=\"0.2.0\" -DPOAC_PROJECT_ROOT=\"${ROOT_DIR}\" -o resolver-test resolver.cpp
./resolver-test
g++ ${BASE_OPT} -o sat-test sat.cpp
./sat-test
g++ ${BASE_OPT} -o semver-test semver.cpp
./semver-test
popd

pushd ./core/stroite/field
g++ ${BASE_OPT} -o standard-test standard.cpp
./standard-test
popd

pushd ./core
g++ ${BASE_OPT} -o exception-test except.cpp
./exception-test
g++ ${BASE_OPT} -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lboost_filesystem -lssl -lcrypto -ldl -lyaml-cpp -DPOAC_VERSION=\"0.2.0\" -DPOAC_PROJECT_ROOT=\"${ROOT_DIR}\" -o infer-test infer.cpp
./infer-test
g++ ${BASE_OPT} -lboost_filesystem -lyaml-cpp -o naming-test naming.cpp
./naming-test
popd

pushd ./util
g++ ${BASE_OPT} -o argparse-test argparse.cpp
./argparse-test
g++ ${BASE_OPT} -o command-test command.cpp
./command-test
g++ ${BASE_OPT} -o misc-test misc.cpp
./misc-test
g++ ${BASE_OPT} -o pretty-test pretty.cpp
./pretty-test
g++ ${BASE_OPT} -o types-test types.cpp
./types-test
popd


popd

# Run gcov and upload report to coveralls
coveralls --exclude build --gcov-options '\-lp' -t ${COVERALLS_TOKEN}
bash <(curl -s https://codecov.io/bash)
