#ifndef SIM_NETWORK_COMPONENT_DOT_H
#define SIM_NETWORK_COMPONENT_DOT_H

#include <SgtSim/SimComponent.h>

#include <SgtCore/Bus.h>
#include <SgtCore/CommonBranch.h>
#include <SgtCore/DgyTransformer.h>
#include <SgtCore/Gen.h>
#include <SgtCore/OverheadLine.h>
#include <SgtCore/SinglePhaseTransformer.h>
#include <SgtCore/YyTransformer.h>
#include <SgtCore/Zip.h>

#include <memory>

namespace SmartGridToolbox
{
   class SimBus : public SimComponentAbc, public Bus 
   {
      public:
         SimBus(Bus&& bus) :
            Bus(std::move(bus))
         {
            // Empty.
         }
   };
   class SimCommonBranch : public SimComponentAbc, public CommonBranch
   {
      public:
         SimCommonBranch(CommonBranch&& branch) :
            CommonBranch(std::move(branch))
         {
            // Empty.
         }
   };
   class SimDgyTransformer : public SimComponentAbc, public DgyTransformer
   {
      public:
         SimDgyTransformer(DgyTransformer&& dgy) :
            DgyTransformer(std::move(dgy))
         {
            // Empty.
         }
   };
   class SimGenericBranch : public SimComponentAbc, public GenericBranch
   {
      public:
         SimGenericBranch(GenericBranch&& branch) :
            GenericBranch(std::move(branch))
         {
            // Empty.
         }
   };
   class SimGenericGen : public SimComponentAbc, public GenericGen
   {
      public:
         SimGenericGen(GenericGen&& gen) :
            GenericGen(std::move(gen))
         {
            // Empty.
         }
   };
   class SimGenericZip : public SimComponentAbc, public GenericZip
   {
      public:
         SimGenericZip(GenericZip&& zip) :
            GenericZip(std::move(zip))
         {
            // Empty.
         }
   };
   class SimOverheadLine : public SimComponentAbc, public OverheadLine
   {
      public:
         SimOverheadLine(OverheadLine&& ohl) :
            OverheadLine(std::move(ohl))
         {
            // Empty.
         }
   };
   class SimSinglePhaseTransformer : public SimComponentAbc, public SinglePhaseTransformer
   {
      public:
         SimSinglePhaseTransformer(SinglePhaseTransformer&& trans) :
            SinglePhaseTransformer(std::move(trans))
         {
            // Empty.
         }
   };
   class SimYyTransformer : public SimComponentAbc, public YyTransformer
   {
      public:
         SimYyTransformer(YyTransformer&& trans) :
            YyTransformer(std::move(trans))
         {
            // Empty.
         }
   };
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
