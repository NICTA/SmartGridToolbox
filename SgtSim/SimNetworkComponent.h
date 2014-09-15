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
   template<typename T> class SimBranch : public SimComponent
   {
      public:

         SimBranch(std::shared_ptr<T> branch) : SimComponent(branch->id()), branch_(branch) {}

         std::shared_ptr<T> branch() {return branch_;}

         std::shared_ptr<const T> branch() const {return std::const_pointer_cast<const T>(branch_);}
         
         static constexpr const char* sComponentType()
         {
            return "sim_branch";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      protected:
         std::shared_ptr<T> branch_;
   };

   template<typename T> class SimBus : public SimComponent
   {
      public:
         SimBus(std::shared_ptr<T> bus) : SimComponent(bus->id()), bus_(bus) {}

         std::shared_ptr<T> bus() {return bus_;}

         std::shared_ptr<const T> bus() const {return std::const_pointer_cast<const T>(bus_);}
         
         static constexpr const char* sComponentType()
         {
            return "sim_bus";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      protected:
         std::shared_ptr<T> bus_;
   };

   template<typename T> class SimGen : public SimComponent
   {
      public:
         SimGen(std::shared_ptr<T> gen) : SimComponent(gen->id()), gen_(gen) {}

         std::shared_ptr<T> gen() {return gen_;}

         std::shared_ptr<const T> gen() const {return std::const_pointer_cast<const T>(gen_);}
         
         static constexpr const char* sComponentType()
         {
            return "sim_gen";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      protected:
         std::shared_ptr<T> gen_;
   };

   template<typename T> class SimZip : public SimComponent
   {
      public:
         SimZip(std::shared_ptr<T> zip) : SimComponent(zip->id()), zip_(zip) {}

         std::shared_ptr<T> zip() {return zip_;}

         std::shared_ptr<const T> zip() const {return std::const_pointer_cast<const T>(zip_);}
         
         static constexpr const char* sComponentType()
         {
            return "sim_zip";
         }

         virtual const char* componentType() const override
         {
            return sComponentType();
         }

      protected:
         std::shared_ptr<T> zip_;
   };

   extern template class SimBranch<BranchAbc>;
   extern template class SimBus<Bus>;
   extern template class SimGen<GenAbc>;
   extern template class SimZip<ZipAbc>;
   
   extern template class SimBranch<CommonBranch>;
   extern template class SimBranch<GenericBranch>;
   extern template class SimBranch<DgyTransformer>;
   extern template class SimBranch<OverheadLine>;
   extern template class SimBranch<SinglePhaseTransformer>;

   extern template class SimGen<GenericGen>;

   extern template class SimZip<GenericZip>;
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
