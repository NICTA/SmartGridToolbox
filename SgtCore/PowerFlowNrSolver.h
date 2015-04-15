#ifndef POWER_FLOW_NR_DOT_H
#define POWER_FLOW_NR_DOT_H

#include "Common.h"
#include "PowerFlowModel.h"
#include "PowerFlowSolver.h"

// Terminology:
// "Bus" and "Branch" refer to n-phase objects i.e. they can contain several phases.
// "Node" and "Link" refer to individual bus conductors and single phase lines.
// A three phase network involving busses and branches can always be decomposed into a single phase network
// involving nodes and links. Thus use of busses and branches is simply a convenience that lumps together nodes and
// links.

namespace Sgt
{
   struct Jacobian
   {
      Jacobian(int nPq, int nPv);

      arma::SpMat<double>& IrPqVrPq()
      {
         return blocks_[0][0];
      }
      arma::SpMat<double>& IiPqVrPq()
      {
         return blocks_[1][0];
      }
      arma::SpMat<double>& IrPvVrPq()
      {
         return blocks_[2][0];
      }
      arma::SpMat<double>& IiPvVrPq()
      {
         return blocks_[3][0];
      }
      arma::SpMat<double>& IrPqViPq()
      {
         return blocks_[0][1];
      }
      arma::SpMat<double>& IiPqViPq()
      {
         return blocks_[1][1];
      }
      arma::SpMat<double>& IrPvViPq()
      {
         return blocks_[2][1];
      }
      arma::SpMat<double>& IiPvViPq()
      {
         return blocks_[3][1];
      }
      arma::SpMat<double>& IrPqVrPv()
      {
         return blocks_[0][2];
      }
      arma::SpMat<double>& IiPqVrPv()
      {
         return blocks_[1][2];
      }
      arma::SpMat<double>& IrPvVrPv()
      {
         return blocks_[2][2];
      }
      arma::SpMat<double>& IiPvVrPv()
      {
         return blocks_[3][2];
      }
      arma::SpMat<double>& IrPqViPv()
      {
         return blocks_[0][3];
      }
      arma::SpMat<double>& IiPqViPv()
      {
         return blocks_[1][3];
      }
      arma::SpMat<double>& IrPvViPv()
      {
         return blocks_[2][3];
      }
      arma::SpMat<double>& IiPvViPv()
      {
         return blocks_[3][3];
      }
      arma::SpMat<double>& IrPqQPv()
      {
         return blocks_[0][4];
      }
      arma::SpMat<double>& IiPqQPv()
      {
         return blocks_[1][4];
      }
      arma::SpMat<double>& IrPvQPv()
      {
         return blocks_[2][4];
      }
      arma::SpMat<double>& IiPvQPv()
      {
         return blocks_[3][4];
      }

      const arma::SpMat<double>& IrPqVrPq() const
      {
         return blocks_[0][0];
      }
      const arma::SpMat<double>& IiPqVrPq() const
      {
         return blocks_[1][0];
      }
      const arma::SpMat<double>& IrPvVrPq() const
      {
         return blocks_[2][0];
      }
      const arma::SpMat<double>& IiPvVrPq() const
      {
         return blocks_[3][0];
      }
      const arma::SpMat<double>& IrPqViPq() const
      {
         return blocks_[0][1];
      }
      const arma::SpMat<double>& IiPqViPq() const
      {
         return blocks_[1][1];
      }
      const arma::SpMat<double>& IrPvViPq() const
      {
         return blocks_[2][1];
      }
      const arma::SpMat<double>& IiPvViPq() const
      {
         return blocks_[3][1];
      }
      const arma::SpMat<double>& IrPqVrPv() const
      {
         return blocks_[0][2];
      }
      const arma::SpMat<double>& IiPqVrPv() const
      {
         return blocks_[1][2];
      }
      const arma::SpMat<double>& IrPvVrPv() const
      {
         return blocks_[2][2];
      }
      const arma::SpMat<double>& IiPvVrPv() const
      {
         return blocks_[3][2];
      }
      const arma::SpMat<double>& IrPqViPv() const
      {
         return blocks_[0][3];
      }
      const arma::SpMat<double>& IiPqViPv() const
      {
         return blocks_[1][3];
      }
      const arma::SpMat<double>& IrPvViPv() const
      {
         return blocks_[2][3];
      }
      const arma::SpMat<double>& IiPvViPv() const
      {
         return blocks_[3][3];
      }
      const arma::SpMat<double>& IrPqQPv()  const
      {
         return blocks_[0][4];
      }
      const arma::SpMat<double>& IiPqQPv()  const
      {
         return blocks_[1][4];
      }
      const arma::SpMat<double>& IrPvQPv()  const
      {
         return blocks_[2][4];
      }
      const arma::SpMat<double>& IiPvQPv()  const
      {
         return blocks_[3][4];
      }

      Array2D<arma::SpMat<double>, 4, 5> blocks_;
   };

   class PowerFlowNrSolver : public PowerFlowSolverInterface
   {
      public:

         virtual bool solve(Network* netw) override;

      private:

         void init(Network* netw);

         unsigned int nPqPv() const
         {
            return mod_->nPq() + mod_->nPv();
         }
         unsigned int nVar() const
         {
            return 2 * nPqPv();
         }

         void initSubmatrixRanges();

         void initJc(Jacobian& Jc) const;

         void calcf(arma::Col<double>& f,
                    const arma::Col<double>& Vr, const arma::Col<double>& Vi,
                    const arma::Col<double>& P, const arma::Col<double>& Q,
                    const arma::Col<double>& M2Pv) const;

         void updateJ(Jacobian& J, const Jacobian& Jc,
                      const arma::Col<double>& Vr, const arma::Col<double>& Vi,
                      const arma::Col<double>& P, const arma::Col<double>& Q,
                      const arma::Col<double>& M2Pv) const;

         void modifyForPv(Jacobian& J, arma::Col<double>& f,
                          const arma::Col<double>& Vr, const arma::Col<double>& Vi,
                          const arma::Col<double>& M2Pv);

         void calcJMatrix(arma::SpMat<double>& JMat, const Jacobian& J) const;

      private:

         Network* netw_;
         std::unique_ptr<PowerFlowModel> mod_;

         arma::Col<arma::uword> selIrPqFrom_f_;
         arma::Col<arma::uword> selIiPqFrom_f_;
         arma::Col<arma::uword> selIrPvFrom_f_;
         arma::Col<arma::uword> selIiPvFrom_f_;
         arma::Col<arma::uword> selVrPqFrom_x_;
         arma::Col<arma::uword> selViPqFrom_x_;
         arma::Col<arma::uword> selQPvFrom_x_;
         arma::Col<arma::uword> selViPvFrom_x_;

         arma::SpMat<double> G_;   ///< Real part of Y matrix.
         arma::SpMat<double> B_;   ///< Imag part of Y matrix.
   };
}

#endif // POWER_FLOW_NR_DOT_H
