#!/bin/bash

cd third_party
./install_third_party.sh $INSTALL_CXX $HOME || exit $?
cd ..

./autogen.sh
./configure CXX=$INSTALL_CXX CPPFLAGS=-I$HOME/include LDFLAGS=-L$HOME/lib --with-klu || exit $?
make || exit $?
cd tests
make tests
./tests > tests.log 2>&1
cat tests.log
cd ..
make check
