#!/bin/sh -l

mkdir build
cd build || exit
if [ "$1" == 'true' ]; then
    cmake .. -Dpoac_BUILD_TEST=ON -DCMAKE_BUILD_TYPE="$2"
else
    cmake .. -Dpoac_BUILD_TEST=OFF -DCMAKE_BUILD_TYPE="$2"
fi
make
[ "$1" == 'true' ] && ctest --output-on-failure --verbose
cd ../
file ./build/poac
[ "$1" == 'true' ] && ./build/poac help
