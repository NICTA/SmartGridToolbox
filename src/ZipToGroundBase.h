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

         virtual const Phases & phases() const {return phases_;}
         virtual Phases & phases() {return phases_;}

         virtual UblasVector<Complex> Y() const = 0;
         virtual UblasVector<Complex> I() const = 0;
         virtual UblasVector<Complex> S() const = 0;
      /// @}
      
      /// @name My private member variables.
      /// @{
         Phases phases_;               ///< My phases on parent bus.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_BASE_DOT_H
