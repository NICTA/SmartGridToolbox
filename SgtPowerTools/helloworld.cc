#include <iostream>

#include "PowerToolsCpp.h"
#include "SgtPowerTools.h"

#include <SgtCore.h>

int main(int argc, char** argv)
{
   using namespace SmartGridToolbox;

   assert(argc == 2);
   std::string fname = argv[1];
   Network nw("network");
   Parser<Network> p;
   p.parse(fname, nw);
   runOpf(nw);
}
