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

#include "OverheadLine.h"

#include "PowerFlow.h"

namespace Sgt
{
    OverheadLine::OverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double L,
            const arma::Mat<double>& condDist, const arma::Col<double> subcondGmr,
            const arma::Col<double>& subcondRPerL, double freq, double rhoEarth,
            const arma::Col<unsigned int>& nInBundle,
            const arma::Col<double>& adjSubcondDist) :
        Component(id),
        BranchAbc(phases0, phases1),
        L_(L),
        condDist_(condDist),
        subcondGmr_(subcondGmr),
        subcondRPerL_(subcondRPerL),
        freq_(freq),
        rhoEarth_(rhoEarth),
        nInBundle_(nInBundle),
        adjSubcondDist_(adjSubcondDist)
    {
        validate(); // TODO: currently, can't adjust any parameters.
    }

    void OverheadLine::validate()
    {
        auto nPhase = phases0().size();
        auto nCond = subcondGmr_.size();

        sgtAssert(!(condDist_.n_rows != nCond || condDist_.n_cols != nCond || subcondRPerL_.size() != nCond),
                "OverheadLine: The conductor distance  matrix must be a square matrix of size nCond x nCond.\n"
                "              The wire resistivity vector must be a vector of size nCond.\n"
                "              The subconductor GMR must be a vector of size nCond.");
        sgtAssert(nCond >= nPhase, "OverheadLine: The number of conductors must be at least the number of phases.");
        sgtAssert(nInBundle_.size() == adjSubcondDist_.size(),
                "OverheadLine: The bundle number vector must have the same number of elements as\n"
                "the bundle distance vector.");

        if (nInBundle_.size() == 0)
        {
            nInBundle_ = arma::Col<unsigned int>(nCond, arma::fill::ones);
            adjSubcondDist_ = arma::Col<double>(nCond, arma::fill::zeros);
        }

        arma::Mat<double> Dij = condDist_;
        for (arma::uword i = 0; i < Dij.n_rows; ++i)
        {
            // TODO: only bother to do one calculation of adjSubcondDist_ doesn't change.
            // Note: handles the unbundled case fine, returning subcondGmr_.
            Dij(i, i) = bundleGmr(nInBundle_(i), subcondGmr_(i), adjSubcondDist_(i));
        }

        arma::Col<double> condRPerL = subcondRPerL_;
        condRPerL = subcondRPerL_ / nInBundle_; // Elementwise division.

        // Calculate the primative impedance matrix, using Carson's equations.
        ZPrim_ = carson(nCond, Dij, subcondRPerL_, L_, freq_, rhoEarth_);

        // Calculate the external Z matrix (i.e. after Kron).
        ZPhase_ = kron(ZPrim_, nPhase);

        // And the nodal admittance matrix
        YNode_ = ZLine2YNode(ZPhase_);
    }
}
