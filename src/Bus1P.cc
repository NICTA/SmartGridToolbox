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

      Bus1P * comp = new Bus1P;
      const std::string nameStr = nd["name"].as<std::string>();
      comp->setName(nameStr);

      const std::string typeStr = nd["type"].as<std::string>();
      if (typeStr == "SL") 
      {
         comp->setType(BusType::SL);
      }
      else if (typeStr == "PQ") 
      {
         comp->setType(BusType::PQ);
      }
      else if (typeStr == "PV") 
      {
         comp->setType(BusType::PV);
      }
      else
      { 
         error() << "Bus type " << typeStr << " is not supported." << std::endl;
         abort();
      }

      if (nd["slack_voltage"])
      {
         comp->setV(nd["slack_voltage"].as<Complex>());
      }

      if (nd["constant_impedance_load"])
      {
         comp->setY(nd["constant_impedance_load"].as<Complex>());
      }

      if (nd["constant_current_load"])
      {
         comp->setI(nd["constant_current_load"].as<Complex>());
      }

      if (nd["constant_power_load"])
      {
         comp->setS(nd["constant_power_load"].as<Complex>());
      }

      if (nd["generator_power"])
      {
         comp->setSGen(nd["generator_power"].as<Complex>());
      }

      mod.addComponent(*comp);
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
         comp->setNetwork(*networkComp);
         networkComp->addBus(*comp);
      }
      else
      {
         error() << "For component " << compNameStr << ", network " << networkStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
   }

   void Bus1P::setLoadYUpdated(const Complex & Y0, const Complex & Y1)
   {
      Y_ += Y1;
      Y_ -= Y0;
      network_->setBusUpdated(*this);
   }

   void Bus1P::setLoadIUpdated(const Complex & I0, const Complex & I1)
   {
      I_ += I1;
      I_ -= I0; 
      network_->setBusUpdated(*this);
   }

   void Bus1P::setLoadSUpdated(const Complex & S0, const Complex & S1)
   {
      S_ += S1;
      S_ -= S0;
      network_->setBusUpdated(*this);
   }
}
