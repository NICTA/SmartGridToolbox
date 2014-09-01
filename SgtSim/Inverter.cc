#include "Inverter.h"

namespace SmartGridToolbox
{
   ublas::vector<Complex> Inverter::S() const
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

   double Inverter::PPerPhase() const
   {
      double P = InverterBase::PPerPhase();
      return std::min(std::abs(P), maxSMagPerPhase_) * (P < 0 ? -1 : 1);
   }

   Inverter::Inverter(const std::string& id, const Phases& phases) :
      InverterBase(id, phases),
      efficiency_(1.0),
      maxSMagPerPhase_(1e9),
      minPowerFactor_(0.0),
      requestedQPerPhase_(0.0),
      inService_(true),
      S_(phases.size(), czero)
   {
      // Empty.
   }
}
