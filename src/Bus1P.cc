#include "Bus1P.h"
#include "Model.h"
#include "Network1P.h"
 
namespace SmartGridToolbox
{
   void Bus1PParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGTDebug("Bus1P : parse.");
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");

      Bus1P & comp = mod.addComponent<Bus1P>();
      const std::string nameStr = nd["name"].as<std::string>();
      comp.setName(nameStr);

      const std::string typeStr = nd["type"].as<std::string>();
      if (typeStr == "SL") 
      {
         comp.setType(BusType::SL);
      }
      else if (typeStr == "PQ") 
      {
         comp.setType(BusType::PQ);
      }
      else if (typeStr == "PV") 
      {
         comp.setType(BusType::PV);
      }
      else
      { 
         error() << "Bus type " << typeStr << " is not supported." << std::endl;
         abort();
      }

      if (nd["slack_voltage"])
      {
         comp.setV(nd["slack_voltage"].as<Complex>());
      }

      if (nd["constant_impedance_load"])
      {
         comp.setY(nd["constant_impedance_load"].as<Complex>());
      }

      if (nd["constant_current_load"])
      {
         comp.setI(nd["constant_current_load"].as<Complex>());
      }

      if (nd["constant_power_load"])
      {
         comp.setS(nd["constant_power_load"].as<Complex>());
      }

      if (nd["generator_power"])
      {
         comp.setSGen(nd["generator_power"].as<Complex>());
      }
   }

   void Bus1PParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGTDebug("Bus1P : postParse.");
      const std::string compNameStr = nd["name"].as<std::string>();
      Bus1P * comp = mod.getComponentNamed<Bus1P>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network1P * networkComp = mod.getComponentNamed<Network1P>(networkStr);
      if (networkComp != nullptr)
      {
         networkComp->addBus(*comp);
      }
      else
      {
         error() << "For component " << compNameStr << ", network " << networkStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
   }

   void Bus1P::updateState(ptime t0, ptime t1)
   {
      Y_ = I_ = S_ = {0.0, 0.0};
      for (const ZipLoad1P * load : zipLoads_)
      {
         Y_ += load->getY();
         I_ += load->getI();
         S_ += load->getS();
      }
   }

   void Bus1P::addZipLoad(ZipLoad1P & zipLoad)
   {
      zipLoads_.push_back(&zipLoad);
      zipLoad.getEventDidUpdate().addAction([&](){getEventNeedsUpdate().trigger();});
      // TODO: this will recalculate all ziploads. Efficiency?
   }
}
