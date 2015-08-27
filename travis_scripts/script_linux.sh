#!/bin/bash

cd third_party/SuiteSparse
sed -i'' -e "s#^INSTALL_LIB.*#INSTALL_LIB = $HOME/lib#g" \
         -e "s#INSTALL_INCLUDE.*#INSTALL_INCLUDE = $HOME/include#g" SuiteSparse_config/SuiteSparse_config.mk
cd SuiteSparse_config; make; make install; cd ..
cd AMD; make; make install; cd ..
cd BTF; make; make install; cd ..
cd COLAMD; make; make install; cd ..
cd KLU; make; make install; cd ..
cd ../..

cd third_party/armadillo-5.400.2
cmake -DCMAKE_CXX_COMPILER=$INSTALL_CXX -DCMAKE_INSTALL_PREFIX=$HOME; make; make install;
cd ../..

cd third_party/yaml-cpp-0.5.1
cmake -DCMAKE_CXX_COMPILER=$INSTALL_CXX -DCMAKE_INSTALL_PREFIX=$HOME; make; make install
cd ../..

./autogen.sh
./configure CXX=$INSTALL_CXX CPPFLAGS=-I$HOME/include LDFLAGS=-L$HOME/lib --with-klu
make; make check
