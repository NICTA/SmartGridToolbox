#!/bin/bash

CXX=$1
INSTALL_PREFIX=$2
echo $CXX $INSTALL_PREFIX
if [ "$CXX" == "" -o "$INSTALL_PREFIX" == "" ]; then
    >&2 echo "Usage: $0 {cxx_command} {install_prefix}"
    exit 1
fi

if [ -w $INSTALL_PREFIX/lib ]; then SUDO=""; else SUDO=sudo; fi

cd armadillo-code
mkdir -p build_sgt
cd build_sgt
if [ ! -f Makefile ]; then
    cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX .. || exit $?
fi
make || exit $?
$SUDO make install || exit $?
cd ../..

cd yaml-cpp
mkdir -p build_sgt
cd build_sgt
if [ ! -f Makefile ]; then
    cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX .. || exit $?
fi
make || exit $?
$SUDO make install || exit $?
cd ../..

cd SuiteSparse
$SUDO make install INSTALL=$INSTALL_PREFIX BLAS=-lblas || exit $?

cd ..
