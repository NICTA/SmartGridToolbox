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
        double P;
        const double c{1.000001};
        sgtLogDebug(LogLevel::VERBOSE) << "Inverter updateState(:): requested PDc = " << reqPDc << endl;
        while (abs(P = PAc(PDc)) > maxSMag_ * c)
        {
            // Limit exceeded. We need to curtail the sources.
            // We want to solve P^2 = maxSMag_^2
            // => PDc^2 * dcToAcFactor(PDc)^2 = maxSMag^2
            // => PDc = sgn(PDc) * maxSMag / dcToAcFactor(PDc)
            // This can be solved by iterative method: PDc_n+1 <= sgn(PDc_n) * maxSMag / dcToAcFactor(PDc_n)
            // For simple efficiencies, should be solved in one step.

            PDc = copysign(maxSMag_ / dcToAcFactor(PDc), PDc);
        }
        sgtLogDebug(LogLevel::VERBOSE) << "Inverter updateState(:): actual PDc = " << PDc << endl;
        double curtailFactor = reqPDc != 0.0 ? PDc / (reqPDc * c) : 1.0;
        sgtLogDebug(LogLevel::VERBOSE) << "Inverter updateState(:): curtail factor = " << curtailFactor << endl;
        for (auto source : sources_)
        {
            auto actualPDc = (curtailFactor * source->requestedPDc());
            sgtLogDebug(LogLevel::VERBOSE) << "Set actual PDc to " << actualPDc << " for " << source->id() << endl;
            source->setActualPDc(actualPDc);
        }
        zip()->setSConst(SConst(P));
    }

    Mat<Complex> Inverter::SConst(double P) const
    {
        using std::copysign;
        using std::min;
        using std::pow;

        double maxSMag2 =  pow(maxSMag_, 2);
        double P2 = pow(P, 2);
        double reqQ2 = pow(requestedQ_, 2);

        double SMag2 = std::min(P2 + reqQ2, maxSMag2);
        double Q = copysign(sqrt(abs(SMag2 - P2)), requestedQ_); 
        // abs is needed in case argument of sqrt is very slightly negative, due to numerical reasons.
        
        uword nPhase = zip()->phases().size();

        if (isDelta_)
        {
            if (nPhase == 3)
            {
                Complex S{-P/3.0, -Q/3.0}; // Load = -ve gen.
                return {{0.0, S, S}, {0.0, 0.0, S}, {0.0, 0.0, 0.0}};
            }
            else if (nPhase == 2)
            {
                Complex S{-P, -Q}; // Load = -ve gen.
                return {{0.0, S}, {0.0, 0.0}};
            }
            else
            {
                sgtError("Wrong number of phases for Delta inverter." << std::endl);
            }
        }
        else
        {
            Complex SLoadPerPh{-P/nPhase, -Q/nPhase}; // Load = -ve gen.
            return diagmat(Col<Complex>(nPhase, fill::none).fill(SLoadPerPh));
        }
    }
}
