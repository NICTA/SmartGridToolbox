#ifndef SIM_NETWORK_COMPONENT_DOT_H
#define SIM_NETWORK_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Branch.h>
#include <SgtCore/Bus.h>
#include <SgtCore/Gen.h>
#include <SgtCore/Zip.h>

namespace SmartGridToolbox
{
   template<typename T> class SimNetworkComponent : public SimComponent
   {
      public:
      /// @name Lifecycle.
      /// @{
         
         SimNetworkComponent(std::shared_ptr<T> component) : component_(component)
         {
            // Empty.
         }

      /// @}
 
      /// @name Component access.
      /// @{
         
         std::shared_ptr<T> component()
         {
            return component_;
         }

      /// @}

      /// @name Overridden member functions from ComponentAbc.
      /// @{
         virtual const std::string& id() const override
         {
            return component_->id();
         }

         virtual const char* componentTypeStr() const override
         {
            return component_->componentTypeStr();
         }

      /// @}

      /// @name Overridden member functions from SimComponent.
      /// @{

      public:
         // virtual Time validUntil() const override;

      protected:
         // virtual void initializeState() override;
         // virtual void updateState(Time t) override;
      
      /// @}
      
      private:

         std::shared_ptr<T> component_;
   };

   using SimBranch = SimNetworkComponent<Branch>;
   using SimBus = SimNetworkComponent<Bus>;
   using SimGen = SimNetworkComponent<GenAbc>;
   using SimZip = SimNetworkComponent<ZipAbc>;
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
