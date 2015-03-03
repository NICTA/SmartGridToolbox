#ifndef SYNCHRONOUS_MACHINE_DOT_H
#define SYNCHRONOUS_MACHINE_DOT_H

#include <iostream>

namespace Sgt
{
   class SynchronousMachine : public Component
   {
      /// @name Overridden member functions from SimComponent.
      /// @{
      
      public:
         // virtual Time validUntil() const override;
         
      protected:
         virtual void initializeState() override;
         virtual void updateState(Time t) override;

      /// @}

      /// @name My member functions.
      /// @{
         
      public:
         SynchronousMachine(const std::string& name);

         void addToNetwork(SimNetwork& nw);

      /// @}

      private:
         SimBus bus_;
         SimBranch branch_;
   }
}

#endif // SYNCHRONOUS_MACHINE_DOT_H
