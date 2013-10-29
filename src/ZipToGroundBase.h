#ifndef ZIP_TO_GROUND_BASE_DOT_H
#define ZIP_TO_GROUND_BASE_DOT_H

#include <smartgridtoolbox/Component.h>
#include <smartgridtoolbox/Parser.h>

namespace SmartGridToolbox
{
   class ZipToGroundBase : public Component
   {
      /// @name My public member functions.
      /// @{
      public:
         ZipToGroundBase(const std::string & name, const Phases & phases) : Component(name), phases_(phases) 
         {
            // Empty.
         }

         virtual const Phases & phases() const {return phases_;}

         virtual ublas::vector<Complex> Y() const = 0;
         virtual ublas::vector<Complex> I() const = 0;
         virtual ublas::vector<Complex> S() const = 0;
      /// @}
      
      /// @name My private member variables.
      /// @{
         Phases phases_;               ///< My phases on parent bus.
      /// @}
   };
}

#endif // ZIP_TO_GROUND_BASE_DOT_H
