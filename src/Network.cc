#include "Network.h"
#include "Bus.h"
#include "Branch.h"
#include "Model.h"
#include <iostream>

namespace SmartGridToolbox
{
   void NetworkParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "Network : parse." << std::endl);

      assertFieldPresent(nd, "name");

      string name = state.expandName(nd["name"].as<std::string>());
      Network & comp = mod.newComponent<Network>(name);
   }

   void Network::updateState(Time t0, Time t1)
   {
      SGT_DEBUG(debug() << "Network : update state." << std::endl);
      solvePowerFlow(); // TODO: inefficient to rebuild even if not needed.
   }

   void Network::addBus(Bus & bus)
   {
      dependsOn(bus);
      busVec_.push_back(&bus);
      busMap_[bus.name()] = &bus;
      bus.didUpdate().addAction([this](){needsUpdate().trigger();}, 
            "Trigger Network " + name() + " needs update");
   }
 
   void Network::addBranch(Branch & branch)
   {
      dependsOn(branch);
      branchVec_.push_back(&branch);
      branch.didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger Network " + name() + " needs update");
   }

   void Network::solvePowerFlow()
   {
      SGT_DEBUG(debug() << "Network : solving power flow." << std::endl);
      SGT_DEBUG(print());
      solver_.reset();
      for (const Bus * bus : busVec_)
      {
         solver_.addBus(bus->name(), bus->type(), bus->phases(), bus->V(), bus->Y(), bus->I(),
                        bus->S());
      }
      for (const Branch * branch : branchVec_)
      {
         solver_.addBranch(branch->bus0().name(), branch->bus1().name(), branch->phases0(),
                           branch->phases1(), branch->Y());
      }

      solver_.validate();

      bool ok = solver_.solve();

      if (ok)
      {
         for (const auto & busPair: solver_.busses())
         {
            Bus * bus = findBus(busPair.second->id_);
            bus->V() = busPair.second->V_; // Push the state back onto bus. We don't want to trigger any events.    
         }
      }
   }

   void Network::print()
   {
      debug() << "Network::print()" << std::endl;
      debug() << "\tBusses:" << std::endl;
      for (const Bus * bus : busVec_)
      {
         debug() << "\t\tBus:" << std::endl;
         debug() << "\t\t\tName   : " << bus->name() << std::endl;
         debug() << "\t\t\tType   : " << bus->type() << std::endl;
         debug() << "\t\t\tPhases : " << bus->phases() << std::endl;
         debug() << "\t\t\tV      : " << bus->V() << std::endl;
         debug() << "\t\t\tY      : " << bus->Y() << std::endl;
         debug() << "\t\t\tI      : " << bus->I() << std::endl;
         debug() << "\t\t\tS      : " << bus->S() << std::endl;
      }
      debug() << "\tBranches:" << std::endl;
      for (const Branch * branch : branchVec_)
      {
         debug() << "\t\tBranch:" << std::endl; 
         debug() << "\t\t\tBus names  : " 
            << branch->bus0().name() << " " << branch->bus1().name() << std::endl;
         debug() << "\t\t\tBus phases : " << branch->phases0() << " " << branch->phases1() << std::endl;
         debug() << "\t\t\tY          :" << std::endl;
         for (int i = 0; i < branch->Y().size1(); ++i)
         {
            debug() << "\t\t\t\t" << std::setw(16) << row(branch->Y(), i) << std::endl;
         }
      }
   }
}
