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

    void Inverter::updateState(const Time& t)
    {
        using std::abs;
        using std::copysign;

        double reqPDc{requestedPDc()};
        double PDc{reqPDc};
        double Pa;
        while (abs(Pa = P(PDc)) > maxSMag_ * 1.000001)
        {
            // Limit exceeded. We need to curtail the sources.
            // We want to solve P^2 = maxSMag_^2
            // => PDc^2 * dcToAcFactor(PDc)^2 = maxSMag^2
            // => PDc = sgn(PDc) * maxSMag / dcToAcFactor(PDc)
            // This can be solved by iterative method: PDc_n+1 <= sgn(PDc_n) * maxSMag / dcToAcFactor(PDc_n)
            // For simple efficiencies, should be solved in one step.

            PDc = copysign(maxSMag_ / dcToAcFactor(PDc), PDc);
        }
        double curtailFactor = PDc / reqPDc;
        for (auto source : sources_)
        {
            source->setActualPDc(curtailFactor * source->requestedPDc());
        }
        zip()->setSConst(SConst(Pa));
    }

    Mat<Complex> Inverter::SConst(double Pa) const
    {
        using std::copysign;
        using std::min;
        using std::pow;

        uword nPhase = zip()->phases().size();

        double maxSMagPerPh =  maxSMag_ / nPhase;
        double maxSMagPerPh2 =  pow(maxSMagPerPh, 2);

        double PPerPh = Pa / nPhase;
        double PPerPh2 = pow(PPerPh, 2);

        double reqQPerPh = requestedQ_ / nPhase;
        double reqQPerPh2 = pow(reqQPerPh, 2);

        double SMagPerPh2 = std::min(PPerPh2 + reqQPerPh2, maxSMagPerPh2);
        double QPerPh = copysign(sqrt(abs(SMagPerPh2 - PPerPh2)), reqQPerPh); 
        // abs is needed in case argument of sqrt is very slightly negative, due to numerical reasons.
        
        Complex SLoadPerPh{-PPerPh, -QPerPh}; // Load = -ve gen.
        return diagmat(Col<Complex>(nPhase, fill::none).fill(SLoadPerPh));
    }
}
