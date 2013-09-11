#include "SimpleInverter.h"

#include "Model.h"
#include "Bus.h"

namespace SmartGridToolbox
{
   void SimpleInverterParser::parse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SimpleInverter : parse." << std::endl);

      assertFieldPresent(nd, "name");
      assertFieldPresent(nd, "bus");
      assertFieldPresent(nd, "phases");

      const std::string nameStr = nd["name"].as<std::string>();
      SimpleInverter & comp = mod.newComponent<SimpleInverter>(nameStr);
      comp.phases() = nd["phases"].as<Phases>();

      if (nd["efficiency"])
      {
         comp.setEfficiency(nd["efficiency"].as<double>());
      }
      else
      {
         comp.setEfficiency(1.0);
      }

      if (nd["max_apparent_power_per_phase"])
      {
         comp.setMaxPAppPerPhase(nd["max_apparent_power_per_phase"].as<double>());
      }
      else
      {
         comp.setMaxPAppPerPhase(10000.0);
      }

      if (nd["min_phase_angle"])
      {
         comp.setMinPhaseAngle(nd["min_phase_angle"].as<double>());
      }
      else
      {
         comp.setMinPhaseAngle(0.0);
      }

      if (nd["requested_reactive_power_per_phase"])
      {
         comp.setRequestedQPerPhase(nd["requested_reactive_power_per_phase"].as<double>());
      }
      else
      {
         comp.setRequestedQPerPhase(0.0);
      }
   }

   void SimpleInverterParser::postParse(const YAML::Node & nd, Model & mod) const
   {
      SGT_DEBUG(debug() << "SimpleInverter : postParse." << std::endl);

      const std::string nameStr = nd["name"].as<std::string>();
      SimpleInverter & comp = *mod.componentNamed<SimpleInverter>(nameStr);

      const std::string busStr = nd["bus"].as<std::string>();
      Bus * busComp = mod.componentNamed<Bus>(busStr);
      if (busComp != nullptr)
      {
         busComp->addZipToGround(comp);
      }
      else
      {
         error() << "For component " << nameStr << ", bus " << busStr << " was not found in the model." 
                 << std::endl;
         abort();
      }
   }

   UblasVector<Complex> SimpleInverter::S() const
   {
      double P2PerPh = PPerPhase(); P2PerPh *= P2PerPh;
      double Q2PerPh = requestedQPerPhase(); Q2PerPh *= Q2PerPh;
      double maxPApp2PerPh =  maxPAppPerPhase_ * maxPAppPerPhase_;
      double PApp2PerPh = std::min(P2PerPh + Q2PerPh, maxPApp2PerPh);
      double QPerPh = sqrt(PApp2PerPh - P2PerPh);
      if (requestedQPerPhase() < 0.0)
      {
         QPerPh *= -1;
      }
      Complex SPerPh{PPerPhase(), QPerPh};
      return UblasVector<Complex>(phases().size(), SPerPh);
   }
}
