#include "Network1P.h"
#include "Bus1P.h"
#include "Branch1P.h"
#include "Model.h"
#include <iostream>

namespace SmartGridToolbox
{
   void Network1PParser::parse(const YAML::Node & nd, Model & mod) const
   {
      debug("Network1P : parse.");
      const std::string nameStr = nd["name"].as<std::string>();
      Network1P * comp = new Network1P;
      comp->setName(nameStr);
      mod.addComponent(*comp);
   }

   void Network1P::updateState(ptime t0, ptime t1)
   {
      // TODO: has network changed? If so, rebuild.
      
      debug("Network1P : update state.");
      bool ok = solver_.solve();
      if (ok)
      {
         for (const Bus1PNR * busNR : solver_.getBusses())
         {
            Bus1P * bus = findBus(busNR->id_);
            bus->setV(busNR->V_);    
         }
         debug("Updated Network1P state. Dumping solver.");
#ifdef DEBUG
         solver_.outputNetwork();
#endif
      }
   }

   void Network1P::rebuildNetwork()
   {
      debug("Network1P : rebuilding network.");
      solver_.reset();
      for (const Bus1P * bus : busVec_)
      {
         solver_.addBus(bus->getName(), bus->getType(), bus->getV(), bus->getY(), bus->getI(), bus->getS());
      }
      for (const Branch1P * branch : branchVec_)
      {
         solver_.addBranch(branch->getBusi().getName(), branch->getBusk().getName(), branch->getY());
         debug("Added branch with Y");
         debug("   %s %s", complex2String(branch->getY()[0][0]).c_str(), complex2String(branch->getY()[0][1]).c_str());
         debug("   %s %s", complex2String(branch->getY()[1][0]).c_str(), complex2String(branch->getY()[1][1]).c_str());
      }
      solver_.validate();
   }
}
