#ifndef TEST_COMPONENT_DOT_H
#define TEST_COMPONENT_DOT_H

#include "Component.h"
#include "Parser.h"

#include <iostream>

namespace SmartGridToolbox
{
   class TestComponentParser : public ComponentParser
   {
      public:
         virtual void parse(const YAML::Node & comp, Model & mod) 
            const override;
         static constexpr const char * getComponentName() 
         {
            return "test_component";
         }
   };

   class TestComponent : public Component
   {
      /// @name Public overridden functions: from Component.
      /// @{
      public:
         virtual ptime getValidUntil() const override 
         {
            return getTime() + seconds(5);
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
         int getValue() {return value_;}
         void setValue(int value) {value_ = value;}
      /// @}

      /// @name My private member variables.
      /// @{
      private:
         int value_;
      /// @}
   };
}

extern SmartGridToolbox::TestComponentParser parser_;

#endif // TEST_COMPONENT_DOT_H
