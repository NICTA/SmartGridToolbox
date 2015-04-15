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

namespace Sgt
{
   class SimBus : public SimComponentAdaptor, public Bus
   {
      public:
         /// Construct a SimBus from a bus.
         /// Note that the list of gens and zips will not be copied; the new SimBus will not be linked to
         /// any gens or zips.
         SimBus(const Bus& bus) :
            Bus(bus)
         {
            removeAllGens();
            removeAllZips();
         }

      protected:
         virtual void initializeState() override
         {
            bool sv = voltageUpdated().isEnabled();
            voltageUpdated().setIsEnabled(false);
            setV(VNom());
            voltageUpdated().setIsEnabled(sv);
         }
   };
   class SimCommonBranch : public SimComponentAdaptor, public CommonBranch
   {
      public:
         SimCommonBranch(const CommonBranch& branch) :
            CommonBranch(branch)
         {
            // Empty.
         }
   };
   class SimDgyTransformer : public SimComponentAdaptor, public DgyTransformer
   {
      public:
         SimDgyTransformer(const DgyTransformer& dgy) :
            DgyTransformer(dgy)
         {
            // Empty.
         }
   };
   class SimGenericBranch : public SimComponentAdaptor, public GenericBranch
   {
      public:
         SimGenericBranch(const GenericBranch& branch) :
            GenericBranch(branch)
         {
            // Empty.
         }
   };
   class SimGenericGen : public SimComponentAdaptor, public GenericGen
   {
      public:
         SimGenericGen(const GenericGen& gen) :
            GenericGen(gen)
         {
            // Empty.
         }
   };
   class SimGenericZip : public SimComponentAdaptor, public GenericZip
   {
      public:
         SimGenericZip(const GenericZip& zip) :
            GenericZip(zip)
         {
            // Empty.
         }
   };
   class SimOverheadLine : public SimComponentAdaptor, public OverheadLine
   {
      public:
         SimOverheadLine(const OverheadLine& ohl) :
            OverheadLine(ohl)
         {
            // Empty.
         }
   };
   class SimSinglePhaseTransformer : public SimComponentAdaptor, public SinglePhaseTransformer
   {
      public:
         SimSinglePhaseTransformer(const SinglePhaseTransformer& trans) :
            SinglePhaseTransformer(trans)
         {
            // Empty.
         }
   };
   class SimYyTransformer : public SimComponentAdaptor, public YyTransformer
   {
      public:
         SimYyTransformer(const YyTransformer& trans) :
            YyTransformer(trans)
         {
            // Empty.
         }
   };
}

#endif // SIM_NETWORK_COMPONENT_DOT_H
