#ifndef BALANCED_POWER_FLOW_NR_DOT_H
#define BALANCED_POWER_FLOW_NR_DOT_H

#include "Common.h"
#include "PowerFlow.h"
#include <vector>

namespace SmartGridToolbox
{
   class NRBus
   {
      public:
         int id;                       ///< Arbitrary bus ID, for external use.
         BusType type;                 ///< bus type (0=PQ, 1=PV, 2=SWING).

         Complex V;                    ///< NRBus voltage / phase.
         Complex Y;                    ///< Constant admittance/phase.
         Complex I;                    ///< Constant current / phase.
         Complex S;                    ///< Constant power / phase.

         int idxPQ;                    ///< My index in list of PQ busses.
   };

   class NRBranch
   {
      public:
         Array2D<Complex, 2, 2> Y;     ///< Complex value of elements in bus admittance matrix in NR solver.
         const NRBus * busi;           ///< My i bus.
         const NRBus * busk;           ///< My k bus.
   };

   // NR solver variables for sparse solution. Solver Ax = b.
   class SolverVars {
      public:
         const unsigned int aNnz;
         const unsigned long int * aRows;
         const unsigned long int * aCols;
         const double * aVals;
         std::vector<double> b;

         SolverVars(ublas::compressed_matrix<double> & m);
   };

   class BalancedPowerFlowNR
   {
      public:
         typedef std::vector<NRBus *> BusVec;
         typedef std::vector<NRBranch *> BranchVec;

      public:
         void addBus(NRBus * bus);
         void addBranch(NRBranch * branch)
         {
            branches_.push_back(branch);
         }
         void validate();
         void solve();

      private:
         typedef ublas::vector_range<vector<double>> VRD; 
         typedef ublas::matrix_range<compressed_matrix<double>> MRD; 

      private:
         void buildBusAdmit();
         void initx();
         void updateBusV_();
         void updateF();
         void updateJ();
         void setSolverVars();

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
         int nVar_;                    ///< Total number of variables.
         /// @}

         /// @name ublas ranges into vectors/matrices.
         /// @{
         ublas::range rPQ_;            ///< Range of PQ busses in list of all busses.
         ublas::range rAll_;           ///< Range of all busses in list of all busses.
                                       /**< Needed for matrix_range. */
         int iSL_;                     ///< Index of slack bus in list of all busses.
         ublas::range rx0_;            ///< Range of real voltage components in x_. 
         ublas::range rx1_;            ///< Range of imag voltage components in x_.
         /// @}

         Complex V0_;                  ///< Slack voltages.

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
         ublas::compressed_matrix<double> JConst_; ///< The part of J that doesn't update at each iteration.
   };
}

#endif // BALANCED_POWER_FLOW_NR_DOT_H
