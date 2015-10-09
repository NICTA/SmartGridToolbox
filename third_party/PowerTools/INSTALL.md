DEPENDENCIES
-------
ipopt: https://projects.coin-or.org/Ipopt

gurobi: http://www.gurobi.com

cmake: http://www.cmake.org (Version 3.2 or better)

********* IMPORTANT *********
--------
PowerTools currently only compiles with gcc 4.9 (due to a lack of c++11 support by Gurobi).
--------

---------------------- For Mac Users ----------------------

If you have brew installed, follow these steps:

>\> brew install gcc49 

>\> mkdir build

>\> cd build

>\> CC=gcc-4.9 CXX=g++-4.9 cmake ..

>\> make

If you have macports installed, follow these steps:

>\> sudo ports install gcc49

>\> sudo port select --set gcc mp-gcc49

>\> mkdir build

>\> cd build

>\> CC=gcc CXX=g++ cmake ..

>\> make

### NOTE ON XCODE
cmake can generate an Xcode project. 

Powertools contains an Xcode plugin (downloaded from https://github.com/hmazhar/xcode-gcc) 
so as to use gcc4.9 as your compiler

To do so, follow these steps:

Close Xcode.

Edit the file utils/GCC\ 4.9.xcplugin/Contents/Resources/GCC\ 4.9.xcspec
Look for the string "ExecPath" and update the path to your gcc-4.9. (by default this is pointing to "/opt/local/bin/gcc"

Enter:
>\> sudo cp -r utils/GCC\ 4.9.xcplugin /Applications/Xcode.app/Contents/Plugins/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins/

>\> mkdir build

>\> cd build

>\> CC=gcc-4.9 CXX=g++-4.9 cmake -G Xcode ..

Open the Xcode project found under ../build

Now you can choose gcc4.9 as your compiler from the Xcode interface.

Remove the -Wmost -Wno-four-char-constants and -Wno-unknown-pragmas flags as these are note supported by gcc4.9


---------------------- For Linux Users ----------------------

>\> sudo apt-get install gcc-4.9 g++-4.9

>\> mkdir build

>\> cd build

>\> CC=gcc-4.9 CXX=g++-.9 cmake ..

>\> make




