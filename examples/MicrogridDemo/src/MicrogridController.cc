#include "MicrogridController.h"

using namespace SmartGridToolbox;

namespace MicrogridDemo
{
   void MicrogridControllerParser::parse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "MicrogridController : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "building_bus");
      assertFieldPresent(nd, "pv_bus");
      assertFieldPresent(nd, "battery");
      assertFieldPresent(nd, "min_P");
      assertFieldPresent(nd, "max_P");
      
      string name = state.expandName(nd["name"].as<std::string>());
      MicrogridController & contr = mod.newComponent<MicrogridController>(name);

      contr.setMinP(nd["min_P"].as<double>());
      contr.setMaxP(nd["max_P"].as<double>());
   }

   void MicrogridControllerParser::postParse(const YAML::Node & nd, Model & mod, const ParserState & state) const
   {
      SGT_DEBUG(debug() << "MicrogridController : postParse." << std::endl);

      string name = state.expandName(nd["name"].as<std::string>());
      MicrogridController * contr = mod.component<MicrogridController>(name);
      
      name = state.expandName(nd["building_bus"].as<std::string>());
      contr->setBuildBus(*mod.component<Bus>(name));
      
      name = state.expandName(nd["pv_bus"].as<std::string>());
      contr->setPvBus(*mod.component<Bus>(name));
      
      name = state.expandName(nd["battery"].as<std::string>());
      contr->setBatt(*mod.component<SimpleBattery>(name));
   }

   void MicrogridController::setBuildBus(SmartGridToolbox::Bus & bus)
   {
      buildBus_ = &bus;
      dependsOn(*buildBus_);
      buildBus_->didUpdate().addAction([this](){needsUpdate().trigger();}, "trigger " + name() + " needsUpdate()");
   }

   void MicrogridController::setPvBus(SmartGridToolbox::Bus & bus)
   {
      pvBus_ = &bus;
      dependsOn(*pvBus_);
      pvBus_->didUpdate().addAction([this](){needsUpdate().trigger();}, "trigger " + name() + " needsUpdate()");
   }

   void MicrogridController::setBatt(SmartGridToolbox::SimpleBattery & batt)
   {
      batt_ = &batt;
      batt_->dependsOn(*this);
      didUpdate().addAction([this](){batt_->needsUpdate().trigger();}, "trigger " + batt_->name() + " needsUpdate()");
   }

   void MicrogridController::updateState(Time t0, Time t1)
   {
      double P = (buildBus_->Sc()(0) + pvBus_->Sc()(0)).real();
      if (P < minP_)
      {
         double battP = 0.5 * (maxP_ + minP_) - P;
         batt_->setRequestedPower(battP);
      }
      else if (P > maxP_)
      {
         double battP = 0.5 * (maxP_ + minP_) - P;
         batt_->setRequestedPower(battP);
      }
      else
      {
         batt_->setRequestedPower(0.0);
      }
   }
}

