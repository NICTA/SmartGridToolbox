#ifndef OUTPUT_DOT_H
#define OUTPUT_DOT_H

#include <iostream>

namespace SmartGridToolbox
{
   inline void Error(const char * message)
   {
      std::cerr << "ERROR: " << message << std::endl;
      throw(message);
   }

   inline void Warning(const char * message)
   {
      std::cerr << "WARNING: " << message << std::endl;
   }

   inline void Message(const char * message)
   {
      std::cout << message << std::endl;
   }
}

#endif // OUTPUT_DOT_H
