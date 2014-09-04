#ifndef SIM_NETWORK_COMPONENT_DOT_H
#define SIM_NETWORK_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Branch.h>
#include <SgtCore/Bus.h>
#include <SgtCore/Gen.h>
#include <SgtCore/Zip.h>

#include <memory>

namespace SmartGridToolbox
{
   /// @brief Base type for simulated network components.
   /// 
   /// This class will not normally be extended by the client. It should only be used through the four
   /// typedefs: SimBranchAbc, SimBus, SimGen, SimZip.
   template<typename T> class SimNetworkComponent : public SimComponentAbc
   {
      public:
      /// @name Lifecycle.
      /// @{
         
         SimNetworkComponent(std::shared_ptr<T> component) : component_(component)
         {
            // Empty.
         }

      /// @}
 
      /// @name Overridden member functions from SimComponentAbc.
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
         
      /// @name Component access.
      /// @{
         
         std::shared_ptr<const T> component() const 
         {
            return std::dynamic_pointer_cast<const T>(component_);
         }

         std::shared_ptr<T> component()
         {
            return std::dynamic_pointer_cast<T>(component_);
         }

      /// @}
      
      /// @name Printing:
      /// @{
         
         virtual void print(std::ostream& os) const override
         {
            os << "sim_network_component:" << std::endl;
            IndentingOStreamBuf _(os);
            os << component_;
         }

      /// @}
      
      private:

         std::shared_ptr<T> component_;
   };
   
   using SimBranchAbc = SimNetworkComponent<BranchAbc>;
   using SimBus = SimNetworkComponent<Bus>;
   using SimGenAbc = SimNetworkComponent<GenAbc>;
   using SimZipAbc = SimNetworkComponent<ZipAbc>;
   
   /// @brief Derived type for derived classes of SimNetworkComponents.
   /// 
   /// For example, if OverheadLine is a child class of BranchAbc, then 
   /// SimNetworkComponentDerived<SimBranchAbc, OverheadLine> is a SimBranchAbc derived class that models an
   /// OverheadLine.
   template<typename P, typename T> class SimNetworkComponentDerived : public P
   {
      public:

         typedef P ParentType;

      /// @name Lifecycle.
      /// @{
         
         SimNetworkComponentDerived(std::shared_ptr<T> component) : ParentType(component)
         {
            // Empty.
         }

      /// @}
 
      /// @name Component access.
      /// @{
         
         std::shared_ptr<const T> component() const 
         {
            return std::dynamic_pointer_cast<const T>(ParentType::component());
         }

         std::shared_ptr<T> component()
         {
            return std::dynamic_pointer_cast<T>(ParentType::component());
         }

      /// @}
   };

   template<typename T> using SimBranchDerived = SimNetworkComponentDerived<SimBranchAbc, T>;
   template<typename T> using SimBusDerived = SimNetworkComponentDerived<SimBus, T>;
   template<typename T> using SimGenDerived = SimNetworkComponentDerived<SimGenAbc, T>;
   template<typename T> using SimZipDerived = SimNetworkComponentDerived<SimZipAbc, T>;
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
