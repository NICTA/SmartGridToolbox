#include <iostream>

#include "PowerToolsCpp/PowerToolsCpp.h"

#include "SgtOpf.h"

int main(int argc, char** argv)
{
   assert(argc == 2);
   std::string fname = argv[1];
   Sgt::Network nw;
   Sgt::Parser::globalParser().parse(fname, nw);
   runOpf(nw);
}
