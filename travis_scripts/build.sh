#!/bin/bash

cd third_party
./install_third_party.sh $CXX /usr/local || exit $?
cd ..

./autogen.sh
./configure CXX=$CXX || exit $?
make || exit $?
cd tests
make tests
./tests > tests.log 2>&1
cat tests.log
cd ..
make VERBOSE=1 check
