#ifndef LOAD_DOT_H
#define LOAD_DOT_H

#include "Common.h"

// TODO : deprecated!

namespace SmartGridToolbox
{
   class Load
   {
      public:
         virtual Complex power() const = 0; 
   };
}

#endif // LOAD_DOT_H
