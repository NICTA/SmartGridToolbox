#ifndef SYNCHRONOUS_MACHINE_DOT_H
#define SYNCHRONOUS_MACHINE_DOT_H

#include <iostream>

namespace SmartGridToolbox
{
   class SynchronousMachine : public Component
   {
      /// @name Overridden member functions from Component.
      /// @{
      
      public:
         virtual Time validUntil() const override {return posix_time::pos_infin}
         
      protected:
         virtual void initializeState() override;
         virtual void updateState(Time t) override;

      /// @}

      /// @name My member functions.
      /// @{
         
      public:
         SynchronousMachine(std::string name);
         virtual ~SynchronousMachine();

      /// @}

      private:
   }
}

#endif // SYNCHRONOUS_MACHINE_DOT_H
