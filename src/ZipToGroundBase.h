#ifndef ZIP_TO_GROUND_BASE_DOT_H
#define ZIP_TO_GROUND_BASE_DOT_H

#include "Component.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class ZipToGroundBase : public Component
   {
      /// @name My public member functions.
      /// @{
      public:
         ZipToGroundBase(const std::string & name) : Component(name) {}

         virtual Phases getPhases() const = 0;
         virtual const UblasVector<Complex> & getY() const = 0;
         virtual const UblasVector<Complex> & getI() const = 0;
         virtual const UblasVector<Complex> & getS() const = 0;
      /// @}
   };
}

#endif // ZIP_TO_GROUND_BASE_DOT_H
