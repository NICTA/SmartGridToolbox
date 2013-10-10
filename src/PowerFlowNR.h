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
               const UblasVector<Complex> & Ys, const UblasVector<Complex> & Ic, const UblasVector<Complex> & S);
         ~BusNR();

         std::string id_;                          ///< Externally relevant id.
         BusType type_;                            ///< Bus type.
         Phases phases_;                           ///< Bus phases.
         UblasVector<Complex> V_;                  ///< Voltage, one per phase.
         UblasVector<Complex> S_;                  ///< Total power injection, one per phase.

         UblasVector<Complex> Ys_;                 ///< Constant admittance shunt, one per phase.
         UblasVector<Complex> Ic_;                 ///< Constant current injection, one per phase.

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
         Complex S_;

         Complex Ys_;
         Complex Ic_;

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

         UblasRange selSLFromAll() const {return {0, nSL_};}
         UblasRange selPQFromAll() const {return {nSL_, nSL_ + nPQ_};}
         UblasRange selPVFromAll() const {return {nSL_ + nPQ_, nSL_ + nPQ_ + nPV_};}
         UblasRange selPQPVFromAll() const {return {nSL_, nSL_ + nPQ_ + nPV_};}
         UblasRange selAllFromAll() const {return {0, nSL_ + nPQ_ + nPV_};}

         /// @name Ordering of variables etc.
         /// @{
         // Matpower ordering for testing:
         UblasRange selIrFromf() const {return {0, nPQ_ + nPV_};}
         UblasRange selIiFromf() const {return {nPQ_ + nPV_, 2 * (nPQ_ + nPV_)};}
         UblasRange selAllFromf() const {return {0, 2 * (nPQ_ + nPV_)};}
         UblasRange selIrPVFromf() const {return {nPQ_, nPQ_ + nPV_};}
         UblasRange selIiPVFromf() const {return {2 * nPQ_ + nPV_, 2 * (nPQ_ + nPV_)};}

         UblasRange selVrFromx() const {return {0, nPQ_ + nPV_};} ///< i.e. as if all busses were PQ.
         UblasRange selViFromx() const {return {nPQ_ + nPV_, 2 * (nPQ_ + nPV_)};} ///< i.e. as if all busses were PQ.
         UblasRange selVrPQFromx() const {return {0, nPQ_};}
         UblasRange selViPQFromx() const {return {nPQ_ + nPV_, 2 * nPQ_ + nPV_};}
         UblasRange selQPVFromx() const {return {nPQ_, nPQ_ + nPV_};}
         UblasRange selViPVFromx() const {return {2 * nPQ_ + nPV_, 2 * (nPQ_ + nPV_)};}

         // Ordering to give a diagonal dominant Jacobian:
         // TODO: assigning into a matrix slice of a sparse matrix destroys the sparsity of the matrix.
         // Thus, for now, we stick with ranges and use the ordering above. 
         /*
         UblasSlice selIrFromf() const {return {1, 2, nPQ_ + nPV_};}
         UblasSlice selIiFromf() const {return {0, 2, nPQ_ + nPV_};}
         UblasSlice selAllFromf() const {return {0, 1, 2 * (nPQ_ + nPV_)};}
         UblasSlice selIrPVFromf() const {return {2 * nPQ_ + 1, 2, nPV_};}
         UblasSlice selIiPVFromf() const {return {2 * nPQ_, 2, nPV_};}

         UblasSlice selVrFromx() const {return {0, 2, nPQ_ + nPV_};} ///< i.e. as if all busses were PQ.
         UblasSlice selViFromx() const {return {1, 2, nPQ_ + nPV_};} ///< i.e. as if all busses were PQ.
         UblasSlice selVrPQFromx() const {return {0, 2, nPQ_};}
         UblasSlice selViPQFromx() const {return {1, 2, nPQ_};}
         UblasSlice selQPVFromx() const {return {2 * nPQ_, 2, nPV_};}
         UblasSlice selViPVFromx() const {return {2 * nPQ_ + 1, 2, nPV_};}
         */
         /// @}
         
         void initV(UblasVector<double> & Vr, UblasVector<double> & Vi) const;
         void initS(UblasVector<double> & P, UblasVector<double> & Q) const;
         void initJC(UblasCMatrix<double> & JC) const;

         void updatef(UblasVector<double> & f,
                      const UblasVector<double> & Vr, const UblasVector<double> & Vi,
                      const UblasVector<double> & P, const UblasVector<double> & Q,
                      const UblasCMatrix<double> & J) const;
         void updateJ(UblasCMatrix<double> & J, const UblasCMatrix<double> & JC,
                      const UblasVector<double> Vr, const UblasVector<double> Vi,
                      const UblasVector<double> P, const UblasVector<double> Q) const;
         void modifyForPV(UblasCMatrix<double> & J, UblasVector<double> f, 
                          const UblasVector<double> Vr, const UblasVector<double> Vi,
                          const UblasVector<double> M2PV);
      private:
         /// @name UblasVector of busses and branches.
         /// @{
         BusMap busses_;
         BranchVec branches_;

         /// @name Array bounds.
         /// @{
         unsigned int nSL_;            ///< Number of slack nodes.
         unsigned int nPQ_;            ///< Number of PQ nodes.
         unsigned int nPV_;            ///< Number of PV nodes.
         /// @}

         // The following are NOT owned by me - they are owned by their parent Busses.
         NodeVec nodes_;
         /// @}

         /// @name Y matrix.
         /// @{
         UblasCMatrix<Complex> Y_;      ///< Real part of Y matrix.
         /// @}

         /// @name Load.
         /// @{
         UblasVector<Complex> Ic_;     ///< Constant current component of load.
         /// @}
   };
}

#endif // POWER_FLOW_NR_DOT_H
