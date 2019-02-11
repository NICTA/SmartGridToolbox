[//]: # (Render using the grip python package: grip --export compiling_and_linking.md)

### To Compile and Link your Executable
SmartGridToolbox is bundled as two libraries: `SgtCore`, and `SgtSim`. To use SmartGridToolbox, your program should include the headers from these libraries. Most headers can be included with the two following `#include` statements:
```
#include <SgtCore.h>
#include <SgtSim.h>
```

If you wish to include individual headers, you can do the following, e.g.:
```
#include <SgtCore/Common.h>
#include <SgtSim/Simulation.h>
```

You will also need to set link to the correct libraries. If you wish to build using GNU autotools, have a look at any of the `tutorials/*/Makefile.am` files. Depending on whether you are interested in simple network solving or discrete event simulation, you could copy and modify e.g. `tutorials/network` or `tutorials/simulation` to use as a template for your project.

Otherwise, the following compile/link command would apply, if using clang++, for example:
```
clang++ {options} -o myprogram myprogram.cc -lSgtSim -lSgtCore -larmadillo -lboost_date_time -lyaml-cpp -lklu -lamd -lcolamd -lbtf
```
