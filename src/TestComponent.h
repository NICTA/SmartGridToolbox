#ifndef TEST_COMPONENT_DOT_H
#define TEST_COMPONENT_DOT_H

#include "Component.h"
#include "Parser.h"

namespace SmartGridToolbox
{
   class TestComponent : public Component
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override {return getTime() + dt_;}
      /// @}

      /// @name Private overridden functions: from Component.
      /// @{
      private:
         virtual void initializeState(ptime t) override;
         virtual void updateState(ptime t0, ptime t1) override;
      /// @}

      /// @name My public member functions.
      /// @{
      public:
         int getVal() {return val_;}
         void setVal(int val) {val_ = val;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         int val_;
      /// @}
   };

   class TestComponentParser : public ComponentParser
   {
      public:
         virtual void parse(const YAML::Node * comps, Model & mod);
   };

}

#endif // TEST_COMPONENT_DOT_H
