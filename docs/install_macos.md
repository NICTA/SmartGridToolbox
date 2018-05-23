# Installing SmartGridToolbox on MacOS
These instructions require a reasonably up to date version of MacOS - we have tested the current version on High Sierra. You will also need to have installed an up to date version of Xcode, as well as [Homebrew](https://brew.sh).

The instructions will get you started with a basic installation of SmartGridToolbox, without extras or extensions.

## Third Party Packages
```
brew update
brew upgrade
brew install cmake
brew install autoconf  
brew install automake
brew install libtool
brew install boost  
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
./install_third_party.sh clang++ /usr/local
```

### Build and install SmartGridToolbox
```
cd ..
./autogen.sh
./configure
make -j4
sudo make install
```
