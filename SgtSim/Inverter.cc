#include "DcPowerSource.h"
#include "Inverter.h"

namespace Sgt
{
    void InverterAbc::addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source)
    {
        sources_.push_back(source);
        dependsOn(source);
    }

    double InverterAbc::availableP() const
    {
        double PDcA = PDc();
        return PDcA * efficiency(PDcA);
    }
        
    void SimpleZipInverter::addDcPowerSource(std::shared_ptr<DcPowerSourceAbc> source)
    {
        InverterAbc::addDcPowerSource(source);
        source->dcPowerChanged().addAction([this]() {injectionChanged().trigger();},
                std::string("Trigger ") + sComponentType() + " " + id() + " injection changed.");
    }


    arma::Col<Complex> SimpleZipInverter::SConst() const
    {
        double PPerPh = availableP() / phases().size();
        double P2PerPh = PPerPh * PPerPh;
        double reqQPerPh = requestedQ_ / phases().size();
        double reqQ2PerPh = reqQPerPh * reqQPerPh;
        double maxSMagPerPh =  maxSMag_ / phases().size();
        double maxSMag2PerPh = maxSMagPerPh * maxSMagPerPh;
        double SMag2PerPh = std::min(P2PerPh + reqQ2PerPh, maxSMag2PerPh);
        double QPerPh = sqrt(SMag2PerPh - P2PerPh);
        if (requestedQ_ < 0.0)
        {
            QPerPh *= -1;
        }
        Complex SPerPh{PPerPh, QPerPh};
        return arma::Col<Complex>(phases().size(), arma::fill::none).fill(SPerPh);
    }

        
}
