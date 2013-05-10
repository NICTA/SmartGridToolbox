#ifndef LOAD_DOT_H
#define LOAD_DOT_H

#include "Common.h"

namespace SmartGridToolbox
{
   class Load
   {
      public:
         virtual Complex getPower() = 0; 
   };
}

#endif // LOAD_DOT_H
