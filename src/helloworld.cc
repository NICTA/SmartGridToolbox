#include "Common.h"
#include <iostream>
#include <complex>
#include <sstream>

int main()
{
   using namespace SmartGridToolbox;
   std::cout << "Hello, world." << std::endl;
   std::cout << "c = " << string2Complex("1 + 2.5i") << std::endl;
   std::cout << "c = " << string2Complex("1 + 2.5j") << std::endl;
   std::cout << "c = " << string2Complex("1+ 2.5j") << std::endl;
   std::cout << "c = " << string2Complex("1 +2.5j") << std::endl;
   std::cout << "c = " << string2Complex("1+2.5j") << std::endl;
   std::cout << "c = " << string2Complex("1 - 2.5i") << std::endl;
   std::cout << "c = " << string2Complex("1 - 2.5j") << std::endl;
   std::cout << "c = " << string2Complex("(1,2.5)") << std::endl;
   std::cout << "c = " << string2Complex("-1 - -2.5j") << std::endl;
}
