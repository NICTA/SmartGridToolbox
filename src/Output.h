#ifndef OUTPUT_DOT_H
#define OUTPUT_DOT_H

#include <iostream>

namespace SmartGridSim
{
   void Error(const char * message)
   {
      std::cerr << "ERROR: " << message << std::endl;
      throw(message);
   }

   void Warning(const char * message)
   {
      std::cerr << "WARNING: " << message << std::endl;
   }

   void Message(const char * message)
   {
      std::cout << message << std::endl;
   }
}

#endif // OUTPUT_DOT_H
