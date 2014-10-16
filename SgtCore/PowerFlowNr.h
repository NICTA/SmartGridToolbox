#ifndef POWER_FLOW_NR_DOT_H
#define POWER_FLOW_NR_DOT_H

#include "Common.h"
#include "PowerFlowModel.h"
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

         PowerFlowNr(PowerFlowModel* prob) :
            mod_(prob)
         {
            // Empty.
         }

         void validate();
         bool solve();

      private:

         unsigned int nPqPv() const {return mod_->nPq() + mod_->nPv();}
         unsigned int nVar() const {return 2 * nPqPv();}

         /// @name Ordering of variables etc.
         /// @{
         // Note: a more elegant general solution to ordering would be to use matrix_slice. But assigning into
         // a matrix slice of a compressed_matrix appears to destroy the sparsity. MatrixRange works, but does not
         // present a general solution to ordering. Thus, when assigning into a compressed_matrix, we need to work
         // element by element, using an indexing scheme.

         int iSl(int i) const {return i;}
         int iPq(int i) const {return mod_->nSl() + i;}
         int iPv(int i) const {return mod_->nSl() + mod_->nPq() + i;}

         ublas::range selSlFromAll() const {return {0, mod_->nSl()};}
         ublas::range selPqFromAll() const {return {mod_->nSl(), mod_->nSl() + mod_->nPq()};}
         ublas::range selPvFromAll() const {return {mod_->nSl() + mod_->nPq(), mod_->nSl() + mod_->nPq() + mod_->nPv()};}
         ublas::range selPqPvFromAll() const {return {mod_->nSl(), mod_->nSl() + mod_->nPq() + mod_->nPv()};}
         ublas::range selAllFromAll() const {return {0, mod_->nSl() + mod_->nPq() + mod_->nPv()};}

         // Note: see above: don't assign into a slice of a sparse matrix!
         ublas::slice selIrPqFrom_f() const {return {1, 2, mod_->nPq()};}
         ublas::slice selIiPqFrom_f() const {return {0, 2, mod_->nPq()};}
         ublas::slice selIrPvFrom_f() const {return {2 * mod_->nPq() + 1, 2, mod_->nPv()};}
         ublas::slice selIiPvFrom_f() const {return {2 * mod_->nPq(), 2, mod_->nPv()};}

         ublas::slice selVrPqFrom_x() const {return {0, 2, mod_->nPq()};}
         ublas::slice selViPqFrom_x() const {return {1, 2, mod_->nPq()};}
         ublas::slice selQPvFrom_x() const {return {2 * mod_->nPq(), 2, mod_->nPv()};}
         ublas::slice selViPvFrom_x() const {return {2 * mod_->nPq() + 1, 2, mod_->nPv()};}
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

         PowerFlowModel* mod_;

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
