#include "Network.h"
#include "Bus.h"
#include "Branch.h"
#include "Model.h"
#include <iostream>

namespace SmartGridToolbox
{
   void NetworkParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "Network : parse." << std::endl);
      const std::string nameStr = nd["name"].as<std::string>();
      Network & comp = mod.newComponent<Network>(nameStr);
   }

   void Network::updateState(ptime t0, ptime t1)
   {
      SGT_DEBUG(debug() << "Network : update state." << std::endl);
      rebuildNetwork(); // TODO: inefficient to rebuild even if not needed.
      bool ok = solver_.solve();
      if (ok)
      {
         for (const auto & busPair: solver_.getBusses())
         {
            Bus * bus = findBus(busPair.second->id_);
            bus->setV(busPair.second->V_); // Push the state back onto bus. We don't want to trigger any events.    
         }
      }
   }

   void Network::rebuildNetwork()
   {
      SGT_DEBUG(debug() << "Network : rebuilding network." << std::endl);
      SGT_DEBUG(print());
      solver_.reset();
      for (const Bus * bus : busVec_)
      {
         solver_.addBus(bus->getName(), bus->getType(), bus->getPhases(), bus->getV(), bus->getY(), bus->getI(),
                        bus->getS());
      }
      for (const Branch * branch : branchVec_)
      {
         solver_.addBranch(branch->getBus0().getName(), branch->getBus1().getName(), branch->getPhases0(),
                           branch->getPhases1(), branch->getY());
      }
      solver_.validate();
   }

   void Network::print()
   {
      debug() << "Network::print()" << std::endl;
      debug() << "\tBusses:" << std::endl;
      for (const Bus * bus : busVec_)
      {
         debug() << "\t\tBus:" << std::endl;
         debug() << "\t\t\tName   : " << bus->getName() << std::endl;
         debug() << "\t\t\tType   : " << bus->getType() << std::endl;
         debug() << "\t\t\tPhases : " << bus->getPhases() << std::endl;
         debug() << "\t\t\tV      : " << bus->getV() << std::endl;
         debug() << "\t\t\tY      : " << bus->getY() << std::endl;
         debug() << "\t\t\tI      : " << bus->getI() << std::endl;
         debug() << "\t\t\tS      : " << bus->getS() << std::endl;
      }
      debug() << "\tBranches:" << std::endl;
      for (const Branch * branch : branchVec_)
      {
         debug() << "\t\tBranch:" << std::endl; 
         debug() << "\t\t\tBus names  : " 
            << branch->getBus0().getName() << " " << branch->getBus1().getName() << std::endl;
         debug() << "\t\t\tBus phases : " << branch->getPhases0() << " " << branch->getPhases1() << std::endl;
         debug() << "\t\t\tY          :" << std::endl;
         for (int i = 0; i < branch->getY().size1(); ++i)
         {
            debug() << "\t\t\t\t" << std::setw(16) << row(branch->getY(), i) << std::endl;
         }
      }
   }
}
