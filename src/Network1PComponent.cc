#include "Network1PComponent.h"
#include "Bus1PComponent.h"
#include "Branch1PComponent.h"
#include "Model.h"
#include <iostream>

namespace SmartGridToolbox
{
   void Network1PComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      const std::string nameStr = nd["name"].as<std::string>();
      Network1PComponent * comp = new Network1PComponent;
      comp->setName(nameStr);
      mod.addComponent(*comp);
   }

   void Network1PComponentParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      const std::string nameStr = nd["name"].as<std::string>();
      Network1PComponent * comp = new Network1PComponent;
      comp->rebuildNetwork();
   }

   void Network1PComponent::rebuildNetwork()
   {
      solver_.reset();
      for (const Bus1PComponent * bus : busses_)
      {
         solver_.addBus(bus->getName(), bus->getType(), bus->getV(), bus->getY(), bus->getI(), bus->getS());
      }
      for (const Branch1PComponent * branch : branches_)
      {
         solver_.addBranch(branch->getBusi().getName(), branch->getBusk().getName(), branch->getY());
      }
      solver_.validate();
   }
}
