#ifndef SIM_NETWORK_COMPONENT_DOT_H
#define SIM_NETWORK_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Branch.h>
#include <SgtCore/Bus.h>
#include <SgtCore/CommonBranch.h>
#include <SgtCore/DgyTransformer.h>
#include <SgtCore/Gen.h>
#include <SgtCore/OverheadLine.h>
#include <SgtCore/SinglePhaseTransformer.h>
#include <SgtCore/Zip.h>

#include <memory>

namespace SmartGridToolbox
{
   /// @brief Base type for simulated network components.
   /// 
   /// A SimNetworkComponent<T> is a SimComponent that has a network component (BranchAbc/Bus/GenAbc/ZipAbc) of type T.
   /// This class will not normally be extended by the client. There are four template instantiations provided, with
   /// the typedefs SimBranchAbc, SimBus, SimGenAbc, SimZipAbc. To use derived network component classes, see
   /// the SimNetworkDerivedComponent<P, T> template class.
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
   
   extern template class SimNetworkComponent<BranchAbc>;
   extern template class SimNetworkComponent<Bus>;
   extern template class SimNetworkComponent<GenAbc>;
   extern template class SimNetworkComponent<ZipAbc>;

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

   extern template class SimNetworkComponentDerived<SimBranchAbc, CommonBranch>;
   extern template class SimNetworkComponentDerived<SimBranchAbc, GenericBranch>;
   extern template class SimNetworkComponentDerived<SimBranchAbc, DgyTransformer>;
   extern template class SimNetworkComponentDerived<SimBranchAbc, OverheadLine>;
   extern template class SimNetworkComponentDerived<SimBranchAbc, SinglePhaseTransformer>;
   extern template class SimNetworkComponentDerived<SimGenAbc, GenericGen>;
   extern template class SimNetworkComponentDerived<SimZipAbc, GenericZip>;

   template<typename T> using SimBranchDerived = SimNetworkComponentDerived<SimBranchAbc, T>;
   template<typename T> using SimBusDerived = SimNetworkComponentDerived<SimBus, T>;
   template<typename T> using SimGenDerived = SimNetworkComponentDerived<SimGenAbc, T>;
   template<typename T> using SimZipDerived = SimNetworkComponentDerived<SimZipAbc, T>;

   using SimCommonBranch = SimBranchDerived<CommonBranch>;
   using SimGenericBranch = SimBranchDerived<GenericBranch>;
   using SimDgyTransformer = SimBranchDerived<DgyTransformer>;
   using SimOverheadLine = SimBranchDerived<OverheadLine>;
   using SimSinglePhaseTransformer = SimBranchDerived<SinglePhaseTransformer>;
   using SimGenericGen = SimZipDerived<GenericGen>;
   using SimGenericZip = SimZipDerived<GenericZip>;
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
