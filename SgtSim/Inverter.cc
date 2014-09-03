#include "DcPowerSource.h"
#include "Inverter.h"

#include <numeric>

namespace SmartGridToolbox
{
   void InverterAbc::addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source)
   {
      dependsOn(source);
      sources_.push_back(source);
      source->didUpdate().addAction([this](){needsUpdate().trigger();},
            "Trigger InverterAbc " + id() + " needs update.");
      // TODO: this will recalculate all zips. Efficiency?
   }

   double InverterAbc::PDc() const
   {
      return std::accumulate(sources_.begin(), sources_.end(), 0.0,
            [] (double tot, const std::shared_ptr<DcPowerSourceAbc>& source) 
            {return tot + source->PDc();});
   }

   ublas::vector<Complex> Inverter::SConst() const
   {
      double PPerPh = PPerPhase();
      double P2PerPh = PPerPh * PPerPh; // Limited by maxSMagPerPhase_.
      double Q2PerPh = requestedQPerPhase_ * requestedQPerPhase_;
      double maxSMag2PerPh =  maxSMagPerPhase_ * maxSMagPerPhase_;
      double SMag2PerPh = std::min(P2PerPh + Q2PerPh, maxSMag2PerPh);
      double QPerPh = sqrt(SMag2PerPh - P2PerPh);
      if (requestedQPerPhase() < 0.0)
      {
         QPerPh *= -1;
      }
      Complex SPerPh{PPerPhase(), QPerPh};
      return ublas::vector<Complex>(phases().size(), SPerPh);
   }

   Inverter::Inverter(const std::string& id, const Phases& phases) :
      InverterAbc(id, phases),
      efficiency_(1.0),
      maxSMagPerPhase_(1e9),
      minPowerFactor_(0.0),
      requestedQPerPhase_(0.0),
      inService_(true),
      S_(phases.size(), czero)
   {
      // Empty.
   }

   double Inverter::PPerPhase() const
   {
      double P = InverterAbc::PPerPhase();
      return std::min(std::abs(P), maxSMagPerPhase_) * (P < 0 ? -1 : 1);
   }

}
