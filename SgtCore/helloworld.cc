#include <iostream>

#include "LibPowerFlow.h"

int main(int argc, char** argv)
{
   assert(argc == 2);
   std::string fname = argv[1];
   LibPowerFlow::Network nw;
   LibPowerFlow::Parser::globalParser().parse(fname, nw);
   std::cout << nw << std::endl;
}
