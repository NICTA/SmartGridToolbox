[//]: # (Render using the grip python package: grip --export install_linux.md)

# Installing SmartGridToolbox on Linux
These instructions require a reasonably up to date version of Linux - for example, Ubuntu 17 (Artful Aardvark) or greater, or Debian 9 (Stretch) or greater. If you would like to install using a slightly older version of Linux, for example, Ubuntu 14 (Trusty Tahr), you will need to make special provisions to compile using a more up to date version of g++ than the default. g++-7 has been tested, and works. While we don't make any guarantees, you may be able to get an idea of how this can be done by looking at SmartGridToolbox's [`.travis.yml`](https://github.com/NICTA/SmartGridToolbox/blob/master/.travis.yml) file.

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
sudo apt-get install libblas-dev
sudo apt-get install liblapack-dev
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

## Compiling and linking your executable
Please see [here](http://nicta.github.io/SmartGridToolbox/compiling_and_linking.html) for instructions about how to compile and link your executable with SmartGridToolbox.
