#!/bin/bash

CXX=$1
INSTALL_PREFIX=$2
echo $CXX $INSTALL_PREFIX
if [ "$CXX" == "" -o "$INSTALL_PREFIX" == "" ]; then
    >&2 echo "Usage: $0 {cxx_command} {install_prefix}"
    exit 1
fi

if [ -w $INSTALL_PREFIX ]; then SUDO=""; else SUDO=sudo; fi

cd armadillo
rm -rf build_sgt
mkdir build_sgt
cd build_sgt
cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX .. || exit $?
make || exit $?
$SUDO make install || exit $?
cd ../..

cd yaml-cpp-0.5.1
rm -rf build_sgt
mkdir build_sgt
cd build_sgt
cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX .. || exit $?
make || exit $?
$SUDO make install || exit $?
cd ../..

cd SuiteSparse
sed -i'' \
    -e "s#^INSTALL_LIB.*#INSTALL_LIB = $INSTALL_PREFIX/lib#g" \
    -e "s#INSTALL_INCLUDE.*#INSTALL_INCLUDE = $INSTALL_PREFIX/include#g" \
    SuiteSparse_config/SuiteSparse_config.mk

cd SuiteSparse_config
make || exit $?
$SUDO make install || exit $?
cd ..

cd AMD
make || exit $?
$SUDO make install || exit $?
cd ..

cd BTF
make || exit $?
$SUDO make install || exit $?
cd ..

cd COLAMD
make || exit $?
$SUDO make install || exit $?
cd ..

cd KLU
make || exit $?
$SUDO make install || exit $?
cd ..

cd ..
