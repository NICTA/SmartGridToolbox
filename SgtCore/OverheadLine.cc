#include "OverheadLine.h"

#include "PowerFlow.h"

namespace Sgt
{
   OverheadLine::OverheadLine(const std::string& id, const Phases& phases0, const Phases& phases1, double L,
                              const arma::Mat<double>& Dij, const arma::Col<double>& resPerL,
                              double rhoEarth, double freq) :
      BranchAbc(id, phases0, phases1),
      L_(L),
      Dij_(Dij),
      resPerL_(resPerL),
      rhoEarth_(rhoEarth),
      freq_(freq)
   {
      validate(); // TODO: currently, can't adjust any parameters.
   }

   void OverheadLine::validate()
   {
      int nPhase = phases0().size();
      int nWire = resPerL_.size();
      if (nWire < nPhase)
      {
         Log().fatal() << "OverheadLine: The number of wires must be at least the number of phases." << std::endl;
      }
      if (Dij_.n_rows != nWire || Dij_.n_cols != nWire)
      {
         Log().fatal() << "OverheadLine: The distance matrix must be a square matrix of size nWire x nWire.\n"
                       "              The wire resistivity vector must be a vector of size nWire." << std::endl;
      }

      // Calculate the primative impedance matrix, using Carson's equations.
      ZPrim_ = carson(nWire, Dij_, resPerL_, L_, freq_, rhoEarth_);

      // Calculate the external Z matrix (i.e. after Kron).
      ZPhase_ = kron(ZPrim_, nPhase);

      // And the nodal admittance matrix
      YNode_ = ZLine2YNode(ZPhase_);
   }
}
