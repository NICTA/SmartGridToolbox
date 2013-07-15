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

   class BalancedPowerFlowNR
   {
      private:
         typedef boost::numeric::ublas::vector<double> VectorDbl;
         typedef boost::numeric::ublas::vector<Complex> VectorCplx;
         typedef boost::numeric::ublas::compressed_matrix<double> CMatrixDbl;
         typedef boost::numeric::ublas::compressed_matrix<Complex> CMatrixCplx;
         typedef boost::numeric::ublas::vector_range<VectorDbl> VectorDblRange;
         typedef boost::numeric::ublas::vector_range<VectorCplx> VectorCplxRange;
         typedef boost::numeric::ublas::matrix_range<CMatrixDbl> CMatrixDblRange;
         typedef boost::numeric::ublas::matrix_range<CMatrixCplx> CMatrixCplxRange;
         typedef boost::numeric::ublas::range Range;

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
         void buildBusAdmit();
         void initx();
         void updateBusV();
         void updateF();
         void updateJ();

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
         Range rPQ_;                   ///< Range of PQ busses in list of all busses.
         Range rAll_;                  ///< Range of all busses in list of all busses.
                                       /**< Needed for matrix_range. */
         int iSL_;                     ///< Index of slack bus in list of all busses.
         Range rx0_;                   ///< Range of real voltage components in x_. 
         Range rx1_;                   ///< Range of imag voltage components in x_.
         /// @}

         Complex V0_;                  ///< Slack voltages.

         /// @name ublas ranges into vectors/matrices.
         /// @{
         VectorDbl PPQ_;               ///< Total power injection of PQ busses.
         VectorDbl QPQ_;

         VectorDbl Vr_;
         VectorDbl Vi_;
         CMatrixCplx Y_;
         CMatrixDbl G_;
         CMatrixDbl B_;

         VectorDbl x_;
         VectorDbl f_;
         CMatrixDbl J_;
         CMatrixDbl JConst_;           ///< The part of J that doesn't update at each iteration.
   };
}

#endif // BALANCED_POWER_FLOW_NR_DOT_H
