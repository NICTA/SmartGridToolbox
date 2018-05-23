# Installing SmartGridToolbox on Linux
These instructions require a reasonably up to date version of Linux - for example, Ubuntu 17 (Artful Aardvark) or greater, or Debian 9 (Stretch) or greater. Working with older versions is possible, but will require special measures to use more up to date version of g++, which can create complications.

The instructions will get you started with a basic installation of SmartGridToolbox, without extras or extensions.

## Third Party Packages
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install gfortran
sudo apt-get install cmake
sudo apt-get install autoconf 
sudo apt-get install automake 
sudo apt-get install libtool 
sudo apt-get install libboost-all-dev
```

## SmartGridToolbox

### Obtain the SmartGridToolbox source
```
git clone -b dev https://github.com/NICTA/SmartGridToolbox.git
```

### Build and install third party dependencies
```
cd SmartGridToolbox
git submodule init
git pull
git submodule update
cd third_party
./install_third_party.sh g++ /usr/local
```

### Build and install SmartGridToolbox
```
cd ..
./autogen.sh
./configure
make -j4
make install
```
