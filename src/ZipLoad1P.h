#ifndef ZIP_LOAD_1P_DOT_H
#define ZIP_LOAD_1P_DOT_H

#include "Component.h"


namespace SmartGridToolbox
{
   class ZipLoad1P : public Component
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return pos_infin;
         }
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override {};
         virtual void updateState(ptime t0, ptime t1) override {};
      /// @}
      
      /// @name My public member functions.
      /// @{
      public:
         const Complex & getY() const {return Y_;}
         void setY(const Complex & Y) {Y_ = Y; getEventNeedsUpdate().trigger();}

         const Complex & getI() const {return I_;}
         void setI(const Complex & I) {I_ = I; getEventNeedsUpdate().trigger();}

         const Complex & getS() const {return S_;}
         void setS(const Complex & S) {S_ = S; getEventNeedsUpdate().trigger();}
      /// @}
      
      /// @name My private member variables.
      /// @{
         Complex Y_;          ///< Constant admittance component.
         Complex I_;          ///< Constant current component.
         Complex S_;          ///< Constant power component.
      /// @}
   };
}

#endif // ZIP_LOAD_1P_DOT_H
