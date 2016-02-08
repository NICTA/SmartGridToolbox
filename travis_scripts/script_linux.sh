#!/bin/bash

cd third_party/cpprestsdk
cmake -DCMAKE_CXX_COMPILER=$INSTALL_CXX -DCMAKE_INSTALL_PREFIX=$HOME -DCMAKE_BUILD_TYPE=Release Release || exit $?
make || exit $?
make install || exit $?
cd ../..

cd third_party/armadillo-5.400.2
cmake -DCMAKE_CXX_COMPILER=$INSTALL_CXX -DCMAKE_INSTALL_PREFIX=$HOME || exit $?
make || exit $?
make install || exit $?
cd ../..

cd third_party/yaml-cpp-0.5.1
cmake -DCMAKE_CXX_COMPILER=$INSTALL_CXX -DCMAKE_INSTALL_PREFIX=$HOME || exit $?
make || exit $?
make install || exit $?
cd ../..

cd third_party/SuiteSparse
sed -i'' -e "s#^INSTALL_LIB.*#INSTALL_LIB = $HOME/lib#g" \
         -e "s#INSTALL_INCLUDE.*#INSTALL_INCLUDE = $HOME/include#g" SuiteSparse_config/SuiteSparse_config.mk
cd SuiteSparse_config
make || exit $?
make install || exit $?
cd ..
cd AMD
make || exit $?
make install || exit $?
cd ..
cd BTF
make || exit $?
make install || exit $?
cd ..
cd COLAMD
make || exit $?
make install || exit $?
cd ..
cd KLU
make || exit $?
make install || exit $?
cd ..
cd ../..

./autogen.sh
./configure CXX=$INSTALL_CXX CPPFLAGS=-I$HOME/include LDFLAGS=-L$HOME/lib --with-klu || exit $?
make || exit $?
make check
