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

   struct BusNR
   {
      BusNR(const std::string & id, BusType type, Phases phases, const ublas::vector<Complex> & V,
            const ublas::vector<Complex> & Ys, const ublas::vector<Complex> & Ic,
            const ublas::vector<Complex> & S);
      ~BusNR();

      std::string id_;                          ///< Externally relevant id.
      BusType type_;                            ///< Bus type.
      Phases phases_;                           ///< Bus phases.
      ublas::vector<Complex> V_;                ///< Voltage, one per phase.
      ublas::vector<Complex> S_;                ///< Total power injection, one per phase.

      ublas::vector<Complex> Ys_;               ///< Constant admittance shunt, one per phase.
      ublas::vector<Complex> Ic_;               ///< Constant current injection, one per phase.

      typedef std::vector<NodeNR *> NodeVec;    ///< Nodes, one per phase.
      NodeVec nodes_;                           ///< Primary list of nodes.
   };

   struct BranchNR
   {
      BranchNR(const std::string & id0, const std::string & id1, Phases phases0, Phases phases1,
            const ublas::matrix<Complex> & Y);

      int nPhase_;                  ///< Number of phases.
      Array<std::string, 2> ids_;   ///< Id of bus 0/1
      Array<Phases, 2> phases_;     ///< phases of bus 0/1.
      ublas::matrix<Complex> Y_;      ///< Bus admittance matrix.
      ublas::matrix<Complex> G_;      ///< Bus admittance matrix.
      ublas::matrix<Complex> B_;      ///< Bus admittance matrix.
   };

   struct NodeNR
   {
      NodeNR(BusNR & bus, int phaseIdx);

      BusNR * bus_;
      int phaseIdx_;

      Complex V_;
      Complex S_;

      Complex Ys_;
      Complex Ic_;

      int idx_;
   };

   struct Jacobian
   {
      Jacobian(int nPQ, int nPV);

      ublas::compressed_matrix<double> & IrPQ_VrPQ() {return blocks_[0][0];}
      ublas::compressed_matrix<double> & IiPQ_VrPQ() {return blocks_[1][0];}
      ublas::compressed_matrix<double> & IrPV_VrPQ() {return blocks_[2][0];}
      ublas::compressed_matrix<double> & IiPV_VrPQ() {return blocks_[3][0];}
      ublas::compressed_matrix<double> & IrPQ_ViPQ() {return blocks_[0][1];}
      ublas::compressed_matrix<double> & IiPQ_ViPQ() {return blocks_[1][1];}
      ublas::compressed_matrix<double> & IrPV_ViPQ() {return blocks_[2][1];}
      ublas::compressed_matrix<double> & IiPV_ViPQ() {return blocks_[3][1];}
      ublas::compressed_matrix<double> & IrPQ_VrPV() {return blocks_[0][2];}
      ublas::compressed_matrix<double> & IiPQ_VrPV() {return blocks_[1][2];}
      ublas::compressed_matrix<double> & IrPV_VrPV() {return blocks_[2][2];}
      ublas::compressed_matrix<double> & IiPV_VrPV() {return blocks_[3][2];}
      ublas::compressed_matrix<double> & IrPQ_ViPV() {return blocks_[0][3];}
      ublas::compressed_matrix<double> & IiPQ_ViPV() {return blocks_[1][3];}
      ublas::compressed_matrix<double> & IrPV_ViPV() {return blocks_[2][3];}
      ublas::compressed_matrix<double> & IiPV_ViPV() {return blocks_[3][3];}
      ublas::compressed_matrix<double> & IrPQ_QPV()  {return blocks_[0][4];}
      ublas::compressed_matrix<double> & IiPQ_QPV()  {return blocks_[1][4];}
      ublas::compressed_matrix<double> & IrPV_QPV()  {return blocks_[2][4];}
      ublas::compressed_matrix<double> & IiPV_QPV()  {return blocks_[3][4];}

      const ublas::compressed_matrix<double> & IrPQ_VrPQ() const {return blocks_[0][0];}
      const ublas::compressed_matrix<double> & IiPQ_VrPQ() const {return blocks_[1][0];}
      const ublas::compressed_matrix<double> & IrPV_VrPQ() const {return blocks_[2][0];}
      const ublas::compressed_matrix<double> & IiPV_VrPQ() const {return blocks_[3][0];}
      const ublas::compressed_matrix<double> & IrPQ_ViPQ() const {return blocks_[0][1];}
      const ublas::compressed_matrix<double> & IiPQ_ViPQ() const {return blocks_[1][1];}
      const ublas::compressed_matrix<double> & IrPV_ViPQ() const {return blocks_[2][1];}
      const ublas::compressed_matrix<double> & IiPV_ViPQ() const {return blocks_[3][1];}
      const ublas::compressed_matrix<double> & IrPQ_VrPV() const {return blocks_[0][2];}
      const ublas::compressed_matrix<double> & IiPQ_VrPV() const {return blocks_[1][2];}
      const ublas::compressed_matrix<double> & IrPV_VrPV() const {return blocks_[2][2];}
      const ublas::compressed_matrix<double> & IiPV_VrPV() const {return blocks_[3][2];}
      const ublas::compressed_matrix<double> & IrPQ_ViPV() const {return blocks_[0][3];}
      const ublas::compressed_matrix<double> & IiPQ_ViPV() const {return blocks_[1][3];}
      const ublas::compressed_matrix<double> & IrPV_ViPV() const {return blocks_[2][3];}
      const ublas::compressed_matrix<double> & IiPV_ViPV() const {return blocks_[3][3];}
      const ublas::compressed_matrix<double> & IrPQ_QPV()  const {return blocks_[0][4];}
      const ublas::compressed_matrix<double> & IiPQ_QPV()  const {return blocks_[1][4];}
      const ublas::compressed_matrix<double> & IrPV_QPV()  const {return blocks_[2][4];}
      const ublas::compressed_matrix<double> & IiPV_QPV()  const {return blocks_[3][4];}

      Array2D<ublas::compressed_matrix<double>, 4, 5> blocks_;
   };

   class PowerFlowNR
   {
      public:
         typedef std::map<std::string, BusNR *> BusMap;  ///< Key is id.
         typedef std::vector<BranchNR *> BranchVec;
         typedef std::vector<NodeNR *> NodeVec;

      public:
         ~PowerFlowNR();

         void addBus(const std::string & id, BusType type, Phases phases, const ublas::vector<Complex> & V,
               const ublas::vector<Complex> & Y, const ublas::vector<Complex> & I, const ublas::vector<Complex> & S);

         const BusMap & busses() const
         {
            return busses_;
         }

         void addBranch(const std::string & idBus0, const std::string & idBus1, Phases phases0, Phases phases1,
                        const ublas::matrix<Complex> & Y);

         const BranchVec & branches() const
         {
            return branches_;
         }

         void reset();
         void validate();
         bool solve();
         void printProblem();

      private:

         unsigned int nPQPV() const {return nPQ_ + nPV_;}
         unsigned int nNode() const {return nSL_ + nPQ_ + nPV_;}
         unsigned int nVar() const {return 2 * (nPQ_ + nPV_);}

         /// @name Ordering of variables etc.
         /// @{
         // Note: a more elegant general solution to ordering would be to use matrix_slice. But assigning into
         // a matrix slice of a compressed_matrix appears to destroy the sparsity. MatrixRange works, but does not
         // present a general solution to ordering. Thus, when assigning into a compressed_matrix, we need to work
         // element by element, using an indexing scheme.

         int iSL(int i) const {return i;}
         int iPQ(int i) const {return nSL_ + i;}
         int iPV(int i) const {return nSL_ + nPQ_ + i;}

         ublas::range selSLFromAll() const {return {0, nSL_};}
         ublas::range selPQFromAll() const {return {nSL_, nSL_ + nPQ_};}
         ublas::range selPVFromAll() const {return {nSL_ + nPQ_, nSL_ + nPQ_ + nPV_};}
         ublas::range selPQPVFromAll() const {return {nSL_, nSL_ + nPQ_ + nPV_};}
         ublas::range selAllFromAll() const {return {0, nSL_ + nPQ_ + nPV_};}

         // Note: see above: don't assign into a slice of a sparse matrix!
         ublas::slice selIrPQFromf() const {return {1, 2, nPQ_};}
         ublas::slice selIiPQFromf() const {return {0, 2, nPQ_};}
         ublas::slice selIrPVFromf() const {return {2 * nPQ_ + 1, 2, nPV_};}
         ublas::slice selIiPVFromf() const {return {2 * nPQ_, 2, nPV_};}

         ublas::slice selVrPQFromx() const {return {0, 2, nPQ_};}
         ublas::slice selViPQFromx() const {return {1, 2, nPQ_};}
         ublas::slice selQPVFromx() const {return {2 * nPQ_, 2, nPV_};}
         ublas::slice selViPVFromx() const {return {2 * nPQ_ + 1, 2, nPV_};}
         /// @}

         void initV(ublas::vector<double> & Vr, ublas::vector<double> & Vi) const;
         void initS(ublas::vector<double> & P, ublas::vector<double> & Q) const;
         void initJC(Jacobian & JC) const;

         void calcf(ublas::vector<double> & f,
                    const ublas::vector<double> & Vr, const ublas::vector<double> & Vi,
                    const ublas::vector<double> & P, const ublas::vector<double> & Q,
                    const ublas::vector<double> & M2PV) const;
         void updateJ(Jacobian & J, const Jacobian & JC,
                      const ublas::vector<double> & Vr, const ublas::vector<double> & Vi,
                      const ublas::vector<double> & P, const ublas::vector<double> & Q,
                      const ublas::vector<double> & M2PV) const;
         void modifyForPV(Jacobian & J, ublas::vector<double> & f,
                          const ublas::vector<double> & Vr, const ublas::vector<double> & Vi,
                          const ublas::vector<double> & M2PV);
         void calcJMatrix(ublas::compressed_matrix<double> & JMat, const Jacobian & J) const;
      private:
         /// @name ublas::vector of busses and branches.
         /// @{
         BusMap busses_;
         BranchVec branches_;

         /// @name Array bounds.
         /// @{
         unsigned int nSL_;   ///< Number of slack nodes.
         unsigned int nPQ_;   ///< Number of PQ nodes.
         unsigned int nPV_;   ///< Number of PV nodes.
         /// @}

         // The following are NOT owned by me - they are owned by their parent Busses.
         NodeVec nodes_;
         /// @}

         /// @name Y matrix.
         /// @{
         ublas::compressed_matrix<Complex> Y_;  ///< Y matrix.
         ublas::compressed_matrix<double> G_;   ///< Real part of Y matrix.
         ublas::compressed_matrix<double> B_;   ///< Imag part of Y matrix.
         /// @}

         /// @name Load.
         /// @{
         ublas::vector<Complex> Ic_;   ///< Constant current component of load.
         /// @}
   };
}

#endif // POWER_FLOW_NR_DOT_H
