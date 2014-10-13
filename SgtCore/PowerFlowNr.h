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
   struct NodeNr;

   struct BusNr
   {
      typedef std::vector<std::unique_ptr<NodeNr>> NodeVec; ///< Nodes, one per phase, owned by me.

      BusNr(const std::string& id, BusType type, const Phases& phases, const ublas::vector<Complex>& V,
            const ublas::vector<Complex>& Ys, const ublas::vector<Complex>& Ic,
            const ublas::vector<Complex>& S);

      std::string id_;            ///< Externally relevant id.
      BusType type_;              ///< Bus type.
      Phases phases_;             ///< Bus phases.
      ublas::vector<Complex> V_;  ///< Voltage, one per phase. Setpoint/warm start on input.
      ublas::vector<Complex> S_;  ///< Total power injection, one per phase. Setpt/wm start on input.

      ublas::vector<Complex> Ys_; ///< Constant admittance shunt, one per phase.
      ublas::vector<Complex> Ic_; ///< Constant current injection, one per phase.
      NodeVec nodes_;             ///< Nodes, one per phase.
   };

   struct BranchNr
   {
      BranchNr(const std::string& id0, const std::string& id1, const Phases& phases0, const Phases& phases1,
            const ublas::matrix<Complex>& Y);

      int nPhase_;                ///< Number of phases.
      Array<std::string, 2> ids_; ///< Id of bus 0/1
      Array<Phases, 2> phases_;   ///< phases of bus 0/1.
      ublas::matrix<Complex> Y_;  ///< Bus admittance matrix.
      ublas::matrix<Complex> G_;  ///< Bus admittance matrix.
      ublas::matrix<Complex> B_;  ///< Bus admittance matrix.
   };

   struct NodeNr
   {
      NodeNr(BusNr& bus, int phaseIdx);

      BusNr* bus_;
      int phaseIdx_;

      Complex V_;
      Complex S_;

      Complex Ys_;
      Complex Ic_;

      int idx_;
   };

   struct Jacobian
   {
      Jacobian(int nPq, int nPv);

      ublas::compressed_matrix<double>& IrPqVrPq() {return blocks_[0][0];}
      ublas::compressed_matrix<double>& IiPqVrPq() {return blocks_[1][0];}
      ublas::compressed_matrix<double>& IrPvVrPq() {return blocks_[2][0];}
      ublas::compressed_matrix<double>& IiPvVrPq() {return blocks_[3][0];}
      ublas::compressed_matrix<double>& IrPqViPq() {return blocks_[0][1];}
      ublas::compressed_matrix<double>& IiPqViPq() {return blocks_[1][1];}
      ublas::compressed_matrix<double>& IrPvViPq() {return blocks_[2][1];}
      ublas::compressed_matrix<double>& IiPvViPq() {return blocks_[3][1];}
      ublas::compressed_matrix<double>& IrPqVrPv() {return blocks_[0][2];}
      ublas::compressed_matrix<double>& IiPqVrPv() {return blocks_[1][2];}
      ublas::compressed_matrix<double>& IrPvVrPv() {return blocks_[2][2];}
      ublas::compressed_matrix<double>& IiPvVrPv() {return blocks_[3][2];}
      ublas::compressed_matrix<double>& IrPqViPv() {return blocks_[0][3];}
      ublas::compressed_matrix<double>& IiPqViPv() {return blocks_[1][3];}
      ublas::compressed_matrix<double>& IrPvViPv() {return blocks_[2][3];}
      ublas::compressed_matrix<double>& IiPvViPv() {return blocks_[3][3];}
      ublas::compressed_matrix<double>& IrPqQPv()  {return blocks_[0][4];}
      ublas::compressed_matrix<double>& IiPqQPv()  {return blocks_[1][4];}
      ublas::compressed_matrix<double>& IrPvQPv()  {return blocks_[2][4];}
      ublas::compressed_matrix<double>& IiPvQPv()  {return blocks_[3][4];}

      const ublas::compressed_matrix<double>& IrPqVrPq() const {return blocks_[0][0];}
      const ublas::compressed_matrix<double>& IiPqVrPq() const {return blocks_[1][0];}
      const ublas::compressed_matrix<double>& IrPvVrPq() const {return blocks_[2][0];}
      const ublas::compressed_matrix<double>& IiPvVrPq() const {return blocks_[3][0];}
      const ublas::compressed_matrix<double>& IrPqViPq() const {return blocks_[0][1];}
      const ublas::compressed_matrix<double>& IiPqViPq() const {return blocks_[1][1];}
      const ublas::compressed_matrix<double>& IrPvViPq() const {return blocks_[2][1];}
      const ublas::compressed_matrix<double>& IiPvViPq() const {return blocks_[3][1];}
      const ublas::compressed_matrix<double>& IrPqVrPv() const {return blocks_[0][2];}
      const ublas::compressed_matrix<double>& IiPqVrPv() const {return blocks_[1][2];}
      const ublas::compressed_matrix<double>& IrPvVrPv() const {return blocks_[2][2];}
      const ublas::compressed_matrix<double>& IiPvVrPv() const {return blocks_[3][2];}
      const ublas::compressed_matrix<double>& IrPqViPv() const {return blocks_[0][3];}
      const ublas::compressed_matrix<double>& IiPqViPv() const {return blocks_[1][3];}
      const ublas::compressed_matrix<double>& IrPvViPv() const {return blocks_[2][3];}
      const ublas::compressed_matrix<double>& IiPvViPv() const {return blocks_[3][3];}
      const ublas::compressed_matrix<double>& IrPqQPv()  const {return blocks_[0][4];}
      const ublas::compressed_matrix<double>& IiPqQPv()  const {return blocks_[1][4];}
      const ublas::compressed_matrix<double>& IrPvQPv()  const {return blocks_[2][4];}
      const ublas::compressed_matrix<double>& IiPvQPv()  const {return blocks_[3][4];}

      Array2D<ublas::compressed_matrix<double>, 4, 5> blocks_;
   };

   class PowerFlowNr
   {
      public:
         typedef std::map<std::string, std::unique_ptr<BusNr>> BusMap;
         typedef std::vector<std::unique_ptr<BranchNr>> BranchVec;
         typedef std::vector<NodeNr*> NodeVec;

      public:
         void addBus(const std::string& id, BusType type, const Phases& phases, const ublas::vector<Complex>& V,
               const ublas::vector<Complex>& Y, const ublas::vector<Complex>& I, const ublas::vector<Complex>& S);

         const BusMap& busses() const
         {
            return busses_;
         }

         void addBranch(const std::string& idBus0, const std::string& idBus1,
               const Phases& phases0, const Phases& phases1, const ublas::matrix<Complex>& Y);

         const BranchVec& branches() const
         {
            return branches_;
         }

         void reset();
         void validate();
         bool solve();
         void printProblem();

      private:

         unsigned int nPqPv() const {return nPq_ + nPv_;}
         unsigned int nNode() const {return nSl_ + nPq_ + nPv_;}
         unsigned int nVar() const {return 2 * (nPq_ + nPv_);}

         /// @name Ordering of variables etc.
         /// @{
         // Note: a more elegant general solution to ordering would be to use matrix_slice. But assigning into
         // a matrix slice of a compressed_matrix appears to destroy the sparsity. MatrixRange works, but does not
         // present a general solution to ordering. Thus, when assigning into a compressed_matrix, we need to work
         // element by element, using an indexing scheme.

         int iSl(int i) const {return i;}
         int iPq(int i) const {return nSl_ + i;}
         int iPv(int i) const {return nSl_ + nPq_ + i;}

         ublas::range selSlFromAll() const {return {0, nSl_};}
         ublas::range selPqFromAll() const {return {nSl_, nSl_ + nPq_};}
         ublas::range selPvFromAll() const {return {nSl_ + nPq_, nSl_ + nPq_ + nPv_};}
         ublas::range selPqPvFromAll() const {return {nSl_, nSl_ + nPq_ + nPv_};}
         ublas::range selAllFromAll() const {return {0, nSl_ + nPq_ + nPv_};}

         // Note: see above: don't assign into a slice of a sparse matrix!
         ublas::slice selIrPqFrom_f() const {return {1, 2, nPq_};}
         ublas::slice selIiPqFrom_f() const {return {0, 2, nPq_};}
         ublas::slice selIrPvFrom_f() const {return {2 * nPq_ + 1, 2, nPv_};}
         ublas::slice selIiPvFrom_f() const {return {2 * nPq_, 2, nPv_};}

         ublas::slice selVrPqFrom_x() const {return {0, 2, nPq_};}
         ublas::slice selViPqFrom_x() const {return {1, 2, nPq_};}
         ublas::slice selQPvFrom_x() const {return {2 * nPq_, 2, nPv_};}
         ublas::slice selViPvFrom_x() const {return {2 * nPq_ + 1, 2, nPv_};}
         /// @}

         void initV(ublas::vector<double>& Vr, ublas::vector<double>& Vi) const;
         void initS(ublas::vector<double>& P, ublas::vector<double>& Q) const;
         void initJc(Jacobian& Jc) const;

         void calcf(ublas::vector<double>& f,
                    const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                    const ublas::vector<double>& P, const ublas::vector<double>& Q,
                    const ublas::vector<double>& M2Pv) const;
         void updateJ(Jacobian& J, const Jacobian& Jc,
                      const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                      const ublas::vector<double>& P, const ublas::vector<double>& Q,
                      const ublas::vector<double>& M2Pv) const;
         void modifyForPv(Jacobian& J, ublas::vector<double>& f,
                          const ublas::vector<double>& Vr, const ublas::vector<double>& Vi,
                          const ublas::vector<double>& M2Pv);
         void calcJMatrix(ublas::compressed_matrix<double>& JMat, const Jacobian& J) const;
      private:
         /// @name ublas::vector of busses and branches.
         /// @{
         BusMap busses_;
         BranchVec branches_;

         /// @name Array bounds.
         /// @{
         unsigned int nSl_;   ///< Number of SL nodes.
         unsigned int nPq_;   ///< Number of PQ nodes.
         unsigned int nPv_;   ///< Number of PV nodes.
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
