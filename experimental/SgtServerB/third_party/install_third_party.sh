cd cpprestsdk
rm -rf build_sgt
mkdir build_sgt
cd build_sgt
cmake -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_SAMPLES=OFF ../Release || exit $?
make || exit $?
sudo make install || exit $?
cd ../..
