#ifndef BALANCED_POWER_FLOW_NR_DOT_H
#define BALANCED_POWER_FLOW_NR_DOT_H

#include "Common.h"
#include "PowerFlow.h"
#include <vector>
#include <map>

namespace SmartGridToolbox
{
   class NRBus
   {
      public:
         std::string id_;              ///< Arbitrary bus ID, for external use.
         BusType type_;                ///< Bus type.
         Complex V_;                   ///< Voltage.
         Complex Y_;                   ///< Constant admittance.
         Complex I_;                   ///< Constant current.
         Complex S_;                   ///< Constant power.

         int idx_;                     ///< My index in list of all busses.
   };

   class NRBranch
   {
      public:
         Array2D<Complex, 2, 2> Y_;    ///< Complex value of elements in bus admittance matrix in NR solver.
         std::string idi_;             ///< id of bus i.
         std::string idk_;             ///< id of bus k.
         const NRBus * busi_;          ///< My i bus.
         const NRBus * busk_;          ///< My k bus.
   };

   class BalancedPowerFlowNR
   {
      public:
         typedef std::vector<NRBus *> BusVec;
         typedef std::map<std::string, NRBus *> BusMap;
         typedef std::vector<NRBranch *> BranchVec;

      public:
         void addBus(const std::string & id, BusType type, Complex V, Complex Y, Complex I, Complex S);
         const BusVec & getBusses()
         {
            return busses_;
         }
         void addBranch(const std::string & idi, const std::string & idk, const Array2D<Complex, 2, 2> & Y);
         const BranchVec & getBranches()
         {
            return branches_;
         }
         void reset();
         void validate();
         bool solve();
         void outputNetwork();

      private:
         void buildBusAdmit();
         void initx();
         void updateBusV();
         void updateF();
         void updateJ();
         void outputCurrentState();

      private:
         /// @name Vectors of busses and branches.
         /// @{
         BusVec busses_;
         BusVec SLBusses_;
         BusVec PQBusses_;
         BusMap bussesById_;

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
         UblasRange rPQ_;              ///< Range of PQ busses in list of all busses.
         UblasRange rAll_;             ///< Range of all busses in list of all busses.
                                       /**< Needed for matrix_range. */
         int iSL_;                     ///< Index of slack bus in list of all busses.
         UblasRange rx0_;              ///< Range of real voltage components in x_. 
         UblasRange rx1_;              ///< Range of imag voltage components in x_.
         /// @}

         Complex V0_;                  ///< Slack voltages.

         UblasVector<double> PPQ_;     ///< Total power injection of PQ busses.
         UblasVector<double> QPQ_;

         UblasVector<double> Vr_;
         UblasVector<double> Vi_;
         UblasCMatrix<Complex> Y_;
         UblasCMatrix<double> G_;
         UblasCMatrix<double> B_;

         UblasVector<double> x_;
         UblasVector<double> f_;
         UblasCMatrix<double> J_;
         UblasCMatrix<double> JConst_;           ///< The part of J that doesn't update at each iteration.
   };

   inline Array2D<Complex, 2, 2> lineY(Complex y)
   {
      return {{{y, -y},{-y, y}}};
   }
}

#endif // BALANCED_POWER_FLOW_NR_DOT_H
