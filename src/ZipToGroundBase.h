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

         virtual Phases getPhases() const {return phases_;}
         virtual void setPhases(Phases phases) {phases_ = phases;}

         virtual UblasVector<Complex> getY() const = 0;
         virtual UblasVector<Complex> getI() const = 0;
         virtual UblasVector<Complex> getS() const = 0;
      /// @}
      
      /// @name My private member variables.
      /// @{
         Phases phases_;               ///< My phases on parent bus.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_BASE_DOT_H
