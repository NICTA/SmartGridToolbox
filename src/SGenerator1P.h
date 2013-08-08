#ifndef S_GENERATOR_1P_DOT_H
#define S_GENERATOR_1P_DOT_H

#include "Component.h"


namespace SmartGridToolbox
{
   class SGenerator1P : public Component
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
         SGenerator1P(const std::string & name) : Component(name) {}

         const Complex & getS() const {return S_;}
         void setS(const Complex & S) {S_ = S;}
      /// @}
      
      /// @name My private member variables.
      /// @{
         Complex S_;          ///< Constant power component.
      /// @}
   };
}

#endif // S_GENERATOR_1P_DOT_H
