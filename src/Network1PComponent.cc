#include "Network1PComponent.h"
#include "Bus1PComponent.h"
#include "Branch1PComponent.h"
#include "Model.h"
#include <iostream>

namespace SmartGridToolbox
{
   void Network1PComponentParser::parse(const YAML::Node & nd, Model & mod) const
   {
      debug("Network1PComponent : parse.");
      const std::string nameStr = nd["name"].as<std::string>();
      Network1PComponent * comp = new Network1PComponent;
      comp->setName(nameStr);
      mod.addComponent(*comp);
   }

   void Network1PComponent::updateState(ptime t0, ptime t1)
   {
      // TODO: has network changed? If so, rebuild.
      
      debug("Network1PComponent : update state.");
      bool ok = solver_.solve();
      if (ok)
      {
         for (const NRBus * busNR : solver_.getBusses())
         {
            Bus1PComponent * bus = findBus(busNR->id_);
            bus->setV(busNR->V_);    
         }
         message("Updated Network1PComponent state. Dumping solver.");
         solver_.outputNetwork();
      }
   }

   void Network1PComponent::rebuildNetwork()
   {
      debug("Network1PComponent : rebuilding network.");
      solver_.reset();
      for (const Bus1PComponent * bus : busVec_)
      {
         solver_.addBus(bus->getName(), bus->getType(), bus->getV(), bus->getY(), bus->getI(), bus->getS());
      }
      for (const Branch1PComponent * branch : branchVec_)
      {
         solver_.addBranch(branch->getBusi().getName(), branch->getBusk().getName(), branch->getY());
         debug("Added branch with Y");
         debug("   %s %s", complex2String(branch->getY()[0][0]).c_str(), complex2String(branch->getY()[0][1]).c_str());
         debug("   %s %s", complex2String(branch->getY()[1][0]).c_str(), complex2String(branch->getY()[1][1]).c_str());
      }
      solver_.validate();
   }
}
