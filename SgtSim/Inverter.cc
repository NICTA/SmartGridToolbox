#include "DcPowerSource.h"
#include "Inverter.h"

namespace SmartGridToolbox
{
   void InverterAbc::addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source)
   {
      sources_.push_back(source);
      dependsOn(source);
      source->dcPowerChanged().addAction([this](){injectionChanged().trigger();},
            std::string("Trigger ") + sComponentType() + " " + id() + " injection changed.");
   }
   
   double InverterAbc::PPerPhase() const
   {
      double PDcA = PDc();
      return PDcA * efficiency(PDcA) / phases().size();
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

   double Inverter::PPerPhase() const
   {
      double P = InverterAbc::PPerPhase();
      return std::min(std::abs(P), maxSMagPerPhase_) * (P < 0 ? -1 : 1);
   }
}
