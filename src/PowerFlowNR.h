#ifndef NR_LOAD_FLOW_DOT_H
#define NR_LOAD_FLOW_DOT_H

#include "Common.h"
#include "PowerFlow.h"
#include <vector>

namespace SmartGridToolbox
{
   class Bus1PNR
   {
      public:
         int id;                       ///< Arbitrary bus ID, for external use.
         BusType type;                 ///< bus type (0=PQ, 1=PV, 2=SWING).

         std::array<Complex, 3> V;     ///< Bus1PNR voltage / phase.
         std::array<Complex, 3> Y;     ///< Constant admittance/phase.
         std::array<Complex, 3> I;     ///< Constant current / phase.
         std::array<Complex, 3> S;     ///< Constant power / phase.

         std::array<double, 3> P;      ///< Real power injection.
         std::array<double, 3> Q;      ///< Reactive power injection.

         int idxPQ;                    ///< My index in list of PQ busses.
   };

   class Branch1PNR
   {
      public:
         Array2D<Complex, 6, 6> Y;     ///< Complex value of elements in bus admittance matrix in NR solver.
         const Bus1PNR * busi;           ///< My i bus.
         const Bus1PNR * busk;           ///< My k bus.
   };

   class PowerFlowNR
   {
      public:
         typedef std::vector<Bus1PNR *> BusVec;
         typedef std::vector<Branch1PNR *> BranchVec;
      public:
         void addBus(Bus1PNR * bus);
         void addBranch(Branch1PNR * branch)
         {
            branches_.push_back(branch);
         }
         void validate();
         void solve();
      private:
         void buildBusAdmit();
         void initx();
         void updateF();
      private:
         /// @name Vectors of busses and branches.
         /// @{
         BusVec busses_;
         BusVec SLBusses_;
         BusVec PQBusses_;

         BranchVec branches_;
         /// @}

         /// @name Array bounds.
         /// @{
         int nSL_;                     ///< Number of slack busses.
         int nPQ_;                     ///< Number of PQ busses.
         int nBus_;                    ///< Total number of busses.
         int nTerm_;                   ///< Total number of bus terminals (each bus has 1 terminal per phase).
         int nVar_;                    ///< Number of variables e.g. length of x_ matrix in NR algorithm.
         int nVarD2_;                  ///< Half nVar_.
         /// @}

         /// @name ublas ranges into vectors/matrices.
         /// @{
         ublas::range rTermPQ(0, 3 * nPQ_);
         ublas::range rTermAll(0, 3 * nPQ_ + 3);
         ublas::range rx1(0, 3 * nPQ_);
         ublas::range rx2(3 * nPQ_, 6 * nPQ_);
         /// @}

         std::array<Complex, 3> V0_;   ///< Slack voltages.

         /// @name ublas ranges into vectors/matrices.
         /// @{
         ublas::vector<double> PPQ_;   ///< Total power injection of PQ busses.
         ublas::vector<double> QPQ_;

         ublas::vector<double> Vr_;
         ublas::vector<double> Vi_;
         ublas::compressed_matrix<Complex> Y_;
         ublas::compressed_matrix<double> G_;
         ublas::compressed_matrix<double> B_;

         ublas::vector<double> x_;
         ublas::vector<double> f_;
         ublas::compressed_matrix<double> J_;
   };
}

#endif // NR_LOAD_FLOW_DOT_H
