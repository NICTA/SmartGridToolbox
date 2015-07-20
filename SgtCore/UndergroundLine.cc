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

#include "UndergroundLine.h"

#include "PowerFlow.h"

namespace Sgt
{
    UndergroundLine::UndergroundLine(const Phases& phases0, const Phases& phases1, double L, bool hasNeutral,
            const arma::Mat<double>& phaseDist, double gmrPhase, double resPerLPhase, double freq, double rhoEarth) :
        BranchAbc(phases0, phases1),
        L_(L),
        hasNeutral_(hasNeutral),
        phaseDist_(phaseDist),
        gmrPhase_(gmrPhase),
        resPerLPhase_(resPerLPhase),
        freq_(freq),
        rhoEarth_(rhoEarth)
    {
        arma::uword nIntPhase = hasNeutral ? phases0.size() + 1 : phases0.size(); // Not including shielding. 
        sgtAssert(phaseDist.n_rows == nIntPhase && phaseDist.n_cols == nIntPhase,
                "UndergroundLine : distance matrix must be size " << std::to_string(nIntPhase)
                << " x " << std::to_string(nIntPhase) << ".");
    }

    UndergroundLineStrandedShield::UndergroundLineStrandedShield(
            const std::string& id, const Phases& phases0, const Phases& phases1, double L, bool hasNeutral,
            const arma::Mat<double>& phaseDist, double gmrPhase, double resPerLPhase, double freq, double rhoEarth,
            double gmrConcStrand, double resPerLConcStrand, int nConcStrands, double rConc) :
        Component(id),
        UndergroundLine(phases0, phases1, L, hasNeutral, phaseDist, gmrPhase, resPerLPhase, freq, rhoEarth),
        gmrConcStrand_(gmrConcStrand),
        resPerLConcStrand_(resPerLConcStrand),
        nConcStrands_(nConcStrands),
        rConc_(rConc)
    {
        // Empty.
    }

    void UndergroundLineStrandedShield::validate()
    {
        auto nExtPhase = phases0().size(); // External phases, no shielding or neutral.
        auto nCond = hasNeutral_ ? 2 * nExtPhase + 1 : 2 * nExtPhase; // Phases, neutral and shielding.
        auto iNeut = nCond - 1;

        // Calculate the distance / GMR matrix Dij.
        arma::Mat<double> Dij(nCond, nCond, arma::fill::zeros);
        double nConcStrInv = 1.0 / nConcStrands_;
        double gmrConc = std::pow(gmrConcStrand_ * nConcStrands_ * std::pow(rConc_, nConcStrands_ - 1), nConcStrInv);
        for (arma::uword i = 0; i < nExtPhase; ++i)
        {
            Dij(i, i) = gmrPhase_; // phase_i - phase_i
            Dij(i + nExtPhase, i + nExtPhase) = gmrConc; // conc_i - conc_i
            Dij(i, i + nExtPhase) = Dij(i + nExtPhase, i) = rConc_; // phase_i - conc_i
            if (hasNeutral_)
            {
                Dij(iNeut, i) = Dij(i, iNeut) = phaseDist_(nExtPhase, i); // phase_i - neutral
                Dij(iNeut, i + nExtPhase) = Dij(i + nExtPhase, iNeut) = 
                    pow(pow(phaseDist_(nExtPhase, i), nConcStrands_) - pow(rConc_, nConcStrands_), nConcStrInv); 
                    // conc_i - neutral
            }
            for (arma::uword j = 0; j < i; ++j)
            {
                Dij(i, j) = Dij(j, i) = phaseDist_(i, j); // phase_i - phase_j
                Dij(i + nExtPhase, j + nExtPhase) = Dij(j + nExtPhase, i + nExtPhase) = phaseDist_(i, j);
                    // conc_i - conc_j
                Dij(i, j + nExtPhase) = Dij(j, i + nExtPhase) = Dij(i + nExtPhase, j) = Dij(j + nExtPhase, i) =
                    pow(pow(phaseDist_(i, j), nConcStrands_) - pow(rConc_, nConcStrands_), nConcStrInv);
                    // phase_i - conc_j
            }
        }
        if (hasNeutral_)
        {
            Dij(iNeut, iNeut) = gmrPhase_; // neutral - neutral
        }

        // Calculate the resistance per unit length.
        arma::Col<double> resPerL(nCond);
        double resPerLConc = resPerLConcStrand_ / nConcStrands_;
        for (arma::uword i = 0; i < nExtPhase; ++i)
        {
            resPerL(i) = resPerLPhase_;
            resPerL(i + nExtPhase) = resPerLConc;
        }
        if (hasNeutral_)
        {
            resPerL(iNeut) = resPerLPhase_;
        }

        // Calculate the primative impedance matrix, using Carson's equations.
        ZPrim_ = carson(nCond, Dij, resPerL, L_, freq_, rhoEarth_);

        // Calculate the external Z matrix (i.e. after Kron).
        ZPhase_ = kron(ZPrim_, nExtPhase);

        // And the nodal admittance matrix
        YNode_ = ZLine2YNode(ZPhase_);
    }
    
    UndergroundLineTapeShield::UndergroundLineTapeShield(
            const std::string& id, const Phases& phases0, const Phases& phases1, double L, bool hasNeutral,
            const arma::Mat<double>& phaseDist, double gmrPhase, double resPerLPhase, double freq, double rhoEarth,
            double outsideRShield, double thickShield, double resistivityShield):
        Component(id),
        UndergroundLine(phases0, phases1, L, hasNeutral, phaseDist, gmrPhase, resPerLPhase, freq, rhoEarth),
        outsideRShield_(outsideRShield),
        thickShield_(thickShield),
        resistivityShield_(resistivityShield)
    {
        // Empty.
    }

    void UndergroundLineTapeShield::validate()
    {
        auto nExtPhase = phases0().size(); // External phases, no shielding or neutral.
        auto nCond = hasNeutral_ ? 2 * nExtPhase + 1 : 2 * nExtPhase; // Phases, neutral and shielding.
        auto iNeut = nCond - 1;

        // Calculate the distance / GMR matrix Dij.
        arma::Mat<double> Dij(nCond, nCond, arma::fill::zeros);
        double gmrShield = outsideRShield_ - 0.5 * thickShield_;
        for (arma::uword i = 0; i < nExtPhase; ++i)
        {
            Dij(i, i) = gmrPhase_; // phase_i - phase_i
            Dij(i + nExtPhase, i + nExtPhase) = gmrShield; // shield_i - shield_i
            Dij(i, i + nExtPhase) = Dij(i + nExtPhase, i) = gmrShield; // phase_i - shield_i
            if (hasNeutral_)
            {
                Dij(iNeut, i) = Dij(i, iNeut) = phaseDist_(nExtPhase, i); // phase_i - neutral
                Dij(iNeut, i + nExtPhase) = Dij(i + nExtPhase, iNeut) = phaseDist_(nExtPhase, i); // shield_i - neutral
            }
            for (arma::uword j = 0; j < i; ++j)
            {
                Dij(i, j) = Dij(j, i) = phaseDist_(i, j); // phase_i - phase_j
                Dij(i + nExtPhase, j + nExtPhase) = Dij(j + nExtPhase, i + nExtPhase) = phaseDist_(i, j);
                    // shield_i - shield_j
                Dij(i, j + nExtPhase) = Dij(j, i + nExtPhase) = Dij(i + nExtPhase, j) = Dij(j + nExtPhase, i) =
                    phaseDist_(i, j); // phase_i - shield_j.
            }
        }
        if (hasNeutral_)
        {
            Dij(iNeut, iNeut) = gmrPhase_; // neutral - neutral
        }

        // Calculate the resistance per unit length.
        arma::Col<double> resPerL(nCond);
        double resPerLShield = resistivityShield_ / (2 * pi * gmrShield * thickShield_);

        for (arma::uword i = 0; i < nExtPhase; ++i)
        {
            resPerL(i) = resPerLPhase_;
            resPerL(i + nExtPhase) = resPerLShield;
        }
        if (hasNeutral_)
        {
            resPerL(iNeut) = resPerLPhase_;
        }

        // Calculate the primative impedance matrix, using Carson's equations.
        ZPrim_ = carson(nCond, Dij, resPerL, L_, freq_, rhoEarth_);

        // Calculate the external Z matrix (i.e. after Kron).
        ZPhase_ = kron(ZPrim_, nExtPhase);

        // And the nodal admittance matrix
        YNode_ = ZLine2YNode(ZPhase_);
    }
}
