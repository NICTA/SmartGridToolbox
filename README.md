# SmartGridToolbox

[![Build Status](https://travis-ci.org/NICTA/SmartGridToolbox.svg?branch=master)](https://travis-ci.org/NICTA/SmartGridToolbox)

[SmartGridToolbox](http://nicta.github.io/SmartGridToolbox) is a C++14 library for electricity grids and associated elements, concentrating on smart/future grids and grid optimisation. It is designed to provide an extensible and flexible starting point for developing a wide variety of smart grid simulations.

## Getting Started

Please see the build instructions for [Linux](http://nicta.github.io/SmartGridToolbox/install_linux.html) and [MacOS](http://nicta.github.io/SmartGridToolbox/install_macos.html). The API reference is available [here](http://nicta.github.io/SmartGridToolbox/doxygen-doc/html/index.html). Additional information and documentation is available at the [SmartGridToolbox homepage](http://nicta.github.io/SmartGridToolbox).

## License

SmartGridToolbox is licensed under the Apache 2.0 License. Please see the [LICENSE](https://github.com/NICTA/SmartGridToolbox/blob/master/LICENSE) file for details. Also see the [NOTICE](https://github.com/NICTA/SmartGridToolbox/blob/master/NOTICE) file (which must be redistributed with derivative works) for details about how to cite useage.

### To compile and link your custom SmartGridToolbox program

If you wish to build using GNU autotools, have a look at examples/PvDemo/Makefile.am. You could copy examples/PvDemo to use as a template for your project.

Otherwise, the following compile command would apply if using clang++, for example:

```
clang++ {options} -o myprogram myprogram.cc -lSgtSim -lSgtCore -larmadillo -lboost_date_time -lyaml-cpp
```

If you are using SuiteSparse for a sparse solver, you need to add extra link
flags:

```
clang++ {options} -o myprogram myprogram.cc -lSgtSim -lSgtCore -larmadillo -lboost_date_time -lyaml-cpp -lklu -lamd -lcolamd -lbtf
```
