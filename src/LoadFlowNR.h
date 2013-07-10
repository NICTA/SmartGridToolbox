#ifndef NR_LOAD_FLOW_DOT_H
#define NR_LOAD_FLOW_DOT_H

#include "Common.h"
#include <vector>

namespace SmartGridToolbox
{
   class NRBus
   {
      public:
         int id;                       ///< Arbitrary bus ID, for external use.
         BusType type;                 ///< bus type (0=PQ, 1=PV, 2=SWING).

         Array<Complex, 3> V;          ///< NRBus voltage / phase.
         Array<Complex, 3> Y;          ///< Constant admittance/phase.
         Array<Complex, 3> I;          ///< Constant current / phase.
         Array<Complex, 3> S;          ///< Constant power / phase.

         Array<double, 3> P;           ///< Real power injection.
         Array<double, 3> Q;           ///< Reactive power injection.

         int idxPQ;                    ///< My index in list of PQ busses.
   };

   class NRBranch
   {
      public:
         Matrix<Complex, 6, 6> Y;      ///< Complex value of elements in bus admittance matrix in NR solver.
         const NRBus * busi;           ///< My i bus.
         const NRBus * busk;           ///< My k bus.
   };

   class NRLoadFlow
   {
      public:
         typedef std::vector<busData *> BusVec;
         typedef std::vector<branchData *> BranchVec;
      public:
         void addBus(busData * bus);
         void addBranch(branchData * branch);
         {
            branches_.push_back(branch);
         }
         void validate();
         void solve();
      private:
         void buildBusAdmit();
      private:
         BusVec busses_;
         BusVec SLBusses_;
         BusVec PQBusses_;

         BranchVec branches_;

         int n_;
         int nSL_;
         int nPQ_;

         Array<Complex, 3> V0_;
         Vector<double> PPQ_;
         Vector<double> QPQ_;
         Vector<double> Vr_;
         Vector<double> Vi_;
         SparseMatrix<Complex> Y_;
         SparseMatrix<double> G_;
         SparseMatrix<double> B_;

         Vector<Complex> x_;
         Vector<Complex> f_;
         SparseMatrix<Complex> J_;
   };
}

#endif // NR_LOAD_FLOW_DOT_H
