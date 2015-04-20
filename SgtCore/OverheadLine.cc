#include "OverheadLine.h"

#include "PowerFlow.h"

namespace Sgt
{
    OverheadLine::OverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double L,
            const arma::Mat<double>& condDist, const arma::Mat<double> subcondGmr,
            const arma::Col<double>& subcondRPerL, double freq, double rhoEarth,
            const arma::Col<int>& nInBundle,
            const arma::Col<double>& adjSubcondDist) :
        BranchAbc(id, phases0, phases1),
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
        int nPhase = phases0().size();
        int nCond = subcondGmr_.size();
        if (condDist_.n_rows != nCond || condDist_.n_cols != nCond || subcondRPerL_.size() != nCond)
        {
            Log().fatal()
                << "OverheadLine: The conductor distance  matrix must be a square matrix of size nCond x nCond.\n"
                   "              The wire resistivity vector must be a vector of size nCond.\n"
                   "              The subconductor GMR must be a vector of size nCond."
                << std::endl;
        }
        if (nCond < nPhase)
        {
            Log().fatal() << "OverheadLine: The number of conductors must be at least the number of phases."
                          << std::endl;
        }
        if (nInBundle_.size() != adjSubcondDist_.size())
        {
            Log().fatal() 
                << "OverheadLine: The bundle number vector must have the same number of elements as\n"
                   "the bundle distance vector." << std::endl;
        }

        arma::Mat<double> Dij = condDist_;
        arma::Col<double> condRPerL = subcondRPerL_;
        if (nInBundle_.size() > 0)
        {
            for (int i = 0; i < Dij.n_rows; ++i)
            {
                // TODO: only bother to do one calculation of adjSubcondDist_ doesn't change.
                Dij(i, i) = bundleGmr(nInBundle_(i), subcondGmr_(i), adjSubcondDist_(i));
            }
            condRPerL = subcondRPerL_ % nInBundle_; // Elementwise mult.
        }

        // Calculate the primative impedance matrix, using Carson's equations.
        ZPrim_ = carson(nCond, Dij, subcondRPerL_, L_, freq_, rhoEarth_);

        // Calculate the external Z matrix (i.e. after Kron).
        ZPhase_ = kron(ZPrim_, nPhase);

        // And the nodal admittance matrix
        YNode_ = ZLine2YNode(ZPhase_);
    }
}
