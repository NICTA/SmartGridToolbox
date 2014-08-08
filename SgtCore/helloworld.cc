#include <iostream>

#include "LibSgtCore.h"

int main(int argc, char** argv)
{
   assert(argc == 2);
   std::string fname = argv[1];
   SmartGridToolbox::Network nw;
   SmartGridToolbox::Parser::globalParser().parse(fname, nw);
   std::cout << nw << std::endl;
}
