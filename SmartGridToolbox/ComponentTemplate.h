#ifndef COMPONENT_TEMPLATE_DOT_H
#define COMPONENT_TEMPLATE_DOT_H

#include <iostream>

namespace SmartGridToolbox
{
   class ComponentTemplate : public Component
   {
      /// @name Lifecycle.
      /// @{
      public:
         ComponentTemplate(std::string name);
      /// @}

      /// @name Public overridden member functions from Component.
      /// @{
      public:
         virtual Time validUntil() const;
      /// @}

      /// @name Protected overridden member functions from Component.
      /// @{
      protected:
         virtual void initializeState();
         virtual void updateState(Time t)
      /// @}
      
      /// @name My public member functions.
      /// @{
      public:
      /// @}
      
      /// @name My private member functions.
      /// @{
      private:
      /// @}
      
      /// @name My private member data.
      /// @{
      private:
      /// @}
   }
}

#endif // COMPONENT_TEMPLATE_DOT_H
