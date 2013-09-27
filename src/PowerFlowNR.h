#ifndef POWER_FLOW_NR_DOT_H
#define POWER_FLOW_NR_DOT_H

#include "Common.h"
#include "PowerFlow.h"
#include <vector>
#include <map>

// Terminology:
// "Bus" and "Branch" refer to n-phase objects i.e. they can contain several phases.
// "Node" and "Link" refer to individual bus conductors and single phase lines.
// A three phase network involving busses and branches can always be decomposed into a single phase network
// involving nodes and links. Thus use of busses and branches is simply a convenience that lumps together nodes and
// links.

namespace SmartGridToolbox
{
   class NodeNR;

   class BusNR
   {
      public:
         BusNR(const std::string & id, BusType type, Phases phases, const UblasVector<Complex> & V,
               const UblasVector<Complex> & Y, const UblasVector<Complex> & I, const UblasVector<Complex> & S);
         ~BusNR();

         std::string id_;                          ///< Externally relevant id.
         BusType type_;                            ///< Bus type.
         Phases phases_;                           ///< Bus phases.
         UblasVector<Complex> V_;                  ///< Voltage, one per phase.
         UblasVector<Complex> Y_;                  ///< Constant admittance shunt, one per phase.
         UblasVector<Complex> I_;                  ///< Constant current injection, one per phase.
         UblasVector<Complex> S_;                  ///< Constant power injection, one per phase.

         typedef std::vector<NodeNR *> NodeVec;    ///< Nodes, one per phase.
         NodeVec nodes_;                           ///< Primary list of nodes.
   };

   class BranchNR
   {
      public:
         BranchNR(const std::string & id0, const std::string & id1, Phases phases0, Phases phases1,
                  const UblasMatrix<Complex> & Y);

         int nPhase_;                  ///< Number of phases.
         Array<std::string, 2> ids_;   ///< Id of bus 0/1
         Array<Phases, 2> phases_;     ///< phases of bus 0/1.
         UblasMatrix<Complex> Y_;      ///< Bus admittance matrix.
   };

   class NodeNR
   {
      public:
         NodeNR(BusNR & bus, int phaseIdx);

         BusNR * bus_;
         int phaseIdx_;

         Complex V_;
         Complex Y_;
         Complex I_;
         Complex S_;

         int idx_;
   };

   class PowerFlowNR
   {
      public:
         typedef std::map<std::string, BusNR *> BusMap;  ///< Key is id.
         typedef std::vector<BranchNR *> BranchVec;
         typedef std::vector<NodeNR *> NodeVec;

      public:
         ~PowerFlowNR();

         void addBus(const std::string & id, BusType type, Phases phases, const UblasVector<Complex> & V,
                     const UblasVector<Complex> & Y, const UblasVector<Complex> & I, const UblasVector<Complex> & S);

         const BusMap & busses() const
         {
            return busses_;
         }

         void addBranch(const std::string & idBus0, const std::string & idBus1, Phases phases0, Phases phases1,
                        const UblasMatrix<Complex> & Y);

         const BranchVec & branches() const
         {
            return branches_;
         }

         void reset();
         void validate();
         bool solve();
         bool printProblem();

      private:

         unsigned int nPQPV() const {return nPQ_ + nPV_;}
         unsigned int nNode() const {return nPQ_ + nPV_ + nSL_;}
         unsigned int nVar() const {return 2 * (nPQ_ + nPV_);}

         UblasRange selPQ() const {return {0, nPQ_};}
         UblasRange selPV() const {return {nPQ_, nPQ_ + nPV_};}
         UblasRange selSL() const {return {nPQ_ + nPV_, nPQ_ + nPV_ + nSL_};}
         UblasRange selPQPV() const {return {0, nPQ_ + nPV_};}
         UblasRange selAll() const {return {0, nPQ_ + nPV_ + nSL_};}

         //UblasSlice selfrPQ() const {return {1, 2, nPQ_};}
         //UblasSlice selfiPQ() const {return {0, 2, nPQ_};}
         //UblasSlice selfrPV() const {return {2 * nPQ_ + 1, 2, nPV_};}
         //UblasSlice selfiPV() const {return {2 * nPQ_, 2, nPV_};}

         //UblasSlice selVrPQ() const {return {0, 2, nPQ_};}
         //UblasSlice selViPQ() const {return {1, 2, nPQ_};}
         //UblasSlice selViPV() const {return {nPQ_, 2, nPV_};}
         //UblasSlice selQPV() const {return {nPQ_+1, 2, nPV_};}

         UblasRange selfrPQ() const {return {0, nPQ_};}
         UblasRange selfiPQ() const {return {nPQ_, 2 * nPQ_};}
         UblasRange selfrPV() const {return {2 * nPQ_, 2 * nPQ_ + nPV_};}
         UblasRange selfiPV() const {return {2 * nPQ_ + nPV_, 2 * (nPQ_ + nPV_)};}

         UblasRange selVrPQ() const {return {0, nPQ_};}
         UblasRange selViPQ() const {return {nPQ_, 2 * nPQ_};}
         UblasRange selViPV() const {return {2 * nPQ_, 2 * nPQ_ + nPV_};}
         UblasRange selQPV() const {return {2 * nPQ_ + nPV_, 2 * (nPQ_ + nPV_)};}
         
         void initV(UblasVector<double> & Vr, UblasVector<double> & Vi) const;
         void initJConst(UblasCMatrix<double> & JConst) const;

         void updatef(UblasVector<double> & f, const UblasVector<double> & Vr, const UblasVector<double> & Vi) const;
         void updateJ(UblasCMatrix<double> & J, const UblasCMatrix<double> & JConst,
                      const UblasVector<double> Vr, const UblasVector<double> Vi) const;

      private:
         /// @name UblasVector of busses and branches.
         /// @{
         BusMap busses_;
         BranchVec branches_;

         /// @name Array bounds.
         /// @{
         unsigned int nPQ_;            ///< Number of PQ nodes.
         unsigned int nPV_;            ///< Number of PV nodes.
         unsigned int nSL_;            ///< Number of slack nodes.
         /// @}

         // The following are NOT owned by me - they are owned by their parent Busses.
         NodeVec nodes_;
         /// @}

         /// @name Constant / supplied quantities.
         /// @{
         UblasVector<double> PPQ_;     ///< Constant real power injection of PQ nodes.
         UblasVector<double> QPQ_;     ///< Constant reactive power injection of PQ nodes.

         UblasVector<double> PPV_;     ///< Constant real power injection of PV nodes.
         UblasVector<double> VPV_;     ///< Constant voltage magnitude of PV nodes.

         UblasVector<double> VSLr_;    ///< Slack voltages real part - one per phase.
         UblasVector<double> VSLi_;    ///< Slack voltages imag part - one per phase.
         /// @}

         /// @name Y matrix.
         /// @{
         UblasCMatrix<double> G_;      ///< Real part of Y matrix.
         UblasCMatrix<double> B_;      ///< Imag part of Y matrix.
         /// @}

         /// @name Load model
         /// @{
         UblasVector<double> P0_;      ///< Const power component of load/gen (power).
         UblasVector<double> Q0_;      ///< Const power component of load/gen (power).
         UblasVector<double> P1_;      ///< Const current component of load/gen (current).
         UblasVector<double> Q1_;      ///< Const current component of load/gen (current).
         UblasVector<double> P2_;      ///< Const impedance component of load (current).
         UblasVector<double> Q2_;      ///< Const impedance component of load (current).
         /// @}
   };
}

#endif // POWER_FLOW_NR_DOT_H
