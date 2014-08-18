#ifndef HAS_ID_DOT_H
#define HAS_ID_DOT_H

#include <string>

namespace SmartGridToolbox
{
   class HasId
   {
      public:
         virtual const std::string& id() const = 0;
   };
}

#endif // HAS_ID_DOT_H
