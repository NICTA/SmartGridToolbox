#ifndef SYNCHRONOUS_MACHINE_DOT_H
#define SYNCHRONOUS_MACHINE_DOT_H

#include <iostream>

namespace SmartGridToolbox
{
   class SynchronousMachine : public Component
   {
      public:
         /// @name Lifecycle.
         /// @{
         SynchronousMachine(std::string name);
         virtual ~SynchronousMachine();
         /// @}

      public:
         /// @name Public overridden member functions from Component.
         /// @{
         virtual Time validUntil() const override;
         /// @}

      protected:
         /// @name Protected overridden member functions from Component.
         /// @{
         virtual void initializeState() override;
         virtual void updateState(Time t) override;
         /// @}

      public:
         /// @name My public member functions.
         /// @{
         /// @}

      private:
         /// @name My private member functions.
         /// @{
         /// @}
      
      private:
         /// @name My private member data.
         /// @{
         /// @}
   }
}

#endif // SYNCHRONOUS_MACHINE_DOT_H
