#!/bin/bash

ROOT_DIR=$PWD
cd ./test/poac/core
g++ -std=c++1z -I${ROOT_DIR}/include -lboost_unit_test_framework -fprofile-arcs -ftest-coverage -o exception-test except.cpp
g++ -std=c++1z -I${ROOT_DIR}/include -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -lboost_unit_test_framework -lboost_filesystem -lssl -lcrypto -ldl -lyaml-cpp -fprofile-arcs -ftest-coverage -DPOAC_VERSION=\"0.2.0\" -DPOAC_PROJECT_ROOT=\"${ROOT_DIR}\" -o infer-test infer.cpp

cd ../../../
coveralls --gcov-options '\-lp' -t ${COVERALLS_TOKEN}
