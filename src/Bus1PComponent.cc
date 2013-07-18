#include "Bus1PComponent.h"
#include "Model.h"
#include "Network1PComponent.h"
 
namespace SmartGridToolbox
{
   void Bus1PComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "network");
      assertFieldPresent(nd, "type");

      Bus1PComponent * comp = new Bus1PComponent;
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
         error("Bus type %s is not supported.", typeStr.c_str());
      }

      if (nd["slack_voltage"])
      {
         const std::string s = nd["slack_voltage"].as<std::string>();
         Complex c = string2Complex(s);
         comp->setV(c);
      }

      if (nd["constant_impedance_load"])
      {
         const std::string s = nd["constant_impedance_load"].as<std::string>();
         Complex c = string2Complex(s);
         comp->setY(c);
      }

      if (nd["constant_current_load"])
      {
         const std::string s = nd["constant_current_load"].as<std::string>();
         Complex c = string2Complex(s);
         comp->setI(c);
      }

      if (nd["constant_power_load"])
      {
         const std::string s = nd["constant_power_load"].as<std::string>();
         Complex c = string2Complex(s);
         comp->setS(c);
      }

      if (nd["generator_power"])
      {
         const std::string s = nd["generator_power"].as<std::string>();
         Complex c = string2Complex(s);
         comp->setSGen(c);
      }

      mod.addComponent(*comp);
   }

   void Bus1PComponentParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      const std::string compNameStr = nd["name"].as<std::string>();
      Bus1PComponent * comp = mod.getComponentNamed<Bus1PComponent>(compNameStr);

      const std::string networkStr = nd["network"].as<std::string>();
      Network1PComponent * networkComp = mod.getComponentNamed<Network1PComponent>(networkStr);
      if (networkComp != nullptr)
      {
         comp->setNetwork(*networkComp);
      }
      else
      {
         error("For component %s, network %s was not found in the model.");
      }
   }
}
