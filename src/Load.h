#ifndef LOAD_DOT_H
#define LOAD_DOT_H

#include "Component.h"

namespace SmartGridToolbox

{
   class Load
   {
      public:
         virtual Complex getPower() override {return Complex(Ph_, 0.0);}
   };
}

#endif // LOAD_DOT_H
