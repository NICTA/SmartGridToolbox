#ifndef COMPONENT_ABC_DOT_H
#define COMPONENT_ABC_DOT_H

#include <string>

using std::string;

namespace SmartGridToolbox
{
   class ComponentAbc
   {
      public:
         virtual ~ComponentAbc() = default;
         virtual const std::string& id() const = 0;
   };
}

#endif // COMPONENT_ABC_DOT_H
