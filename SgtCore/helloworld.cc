#include <iostream>

#include "LibSgtCore.h"

int main(int argc, char** argv)
{
   using namespace SmartGridToolbox;

   assert(argc == 2);
   std::string fname = argv[1];
   Network nw("hello_network", 100.0);
   SmartGridToolbox::Parser::globalParser().parse(fname, nw);
   std::cout << nw << std::endl;
}
