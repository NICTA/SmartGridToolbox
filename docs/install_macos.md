[//]: # (Render using the grip python package: grip --export install_macos.md)

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
git clone https://github.com/NICTA/SmartGridToolbox.git
cd SmartGridToolbox
git submodule init
git submodule update
```

### Build and install third party dependencies
```
cd third_party
./install_third_party.sh clang++ /usr/local
cd ..
```

### Build and install SmartGridToolbox
```
./autogen.sh
./configure
make -j4
make install
```

## Compiling and linking your executable
Please see [here](http://nicta.github.io/SmartGridToolbox/compiling_and_linking.html) for instructions about how to compile and link your executable with SmartGridToolbox.
