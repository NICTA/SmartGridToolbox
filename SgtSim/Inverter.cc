// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "DcPowerSource.h"
#include "Inverter.h"

#include <SgtCore/Zip.h>

using namespace arma;

namespace Sgt
{
    void InverterAbc::addDcPowerSource(const ConstSimComponentPtr<DcPowerSourceAbc>& source)
    {
        sources_.push_back(source);
        dependsOn(source, true);
    }

    double InverterAbc::availableP() const
    {
        double PDcA = PDc();
        return PDcA >= 0 ? PDcA * efficiency(PDcA) : PDcA / efficiency(PDcA);
    }
        
    void Inverter::addDcPowerSource(const ConstSimComponentPtr<DcPowerSourceAbc>& source)
    {
        InverterAbc::addDcPowerSource(source);
    }
    
    void Inverter::updateState(const Time& t)
    {
        zip()->setSConst(SConst());
    }

    Mat<Complex> Inverter::SConst() const
    {
        using std::abs;
        using std::copysign;
        using std::min;
        using std::pow;

        uword nPhase = zip()->phases().size();

        double maxSMagPerPh =  maxSMag_ / nPhase;
        double maxSMagPerPh2 =  pow(maxSMagPerPh, 2);

        double availPPerPh = availableP() / nPhase;
        double PPerPh = copysign(min(availPPerPh, maxSMagPerPh), availPPerPh); 
        double PPerPh2 = pow(PPerPh, 2);

        double reqQPerPh = requestedQ_ / nPhase;
        double reqQPerPh2 = pow(reqQPerPh, 2);

        double SMagPerPh2 = std::min(PPerPh2 + reqQPerPh2, maxSMagPerPh2);
        double QPerPh = copysign(sqrt(SMagPerPh2 - PPerPh2), reqQPerPh);
        
        Complex SLoadPerPh{-PPerPh, -QPerPh}; // Load = -ve gen.
        return diagmat(Col<Complex>(nPhase, fill::none).fill(SLoadPerPh));
    }
}
