#!/bin/bash

cd third_party
./install_third_party.sh $INSTALL_CXX $HOME || exit $?

./autogen.sh
./configure CXX=$INSTALL_CXX CPPFLAGS=-I$HOME/include LDFLAGS=-L$HOME/lib --with-klu || exit $?
make || exit $?
make check
