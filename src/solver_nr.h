#ifndef SOLVER_NR_DOT_H
#define SOLVER_NR_DOT_H

#include "Common.h"
#include "PowerFlow.h"
#include <vector>

namespace SmartGridToolbox
{
   struct  NrBusData
   {
      BusType type;                    ///< bus type (0=PQ, 1=PV, 2=SWING).
      unsigned char phases;            ///< Phases property. Used for construction of matrices (skip bad entries).
                                       /**  0x01 = C
                                        *   0x02 = B
                                        *   0x04 = A
                                        *   0x08 = Delta (== ABC) */

      // The following appear to be A/B/C to ground for wye and AB/BC/CA for delta.
      Array<Complex, 3> V;             ///< Bus voltage / phase (wye) or phase pair (delta).
      Array<Complex, 3> S;             ///< Constant power / phase (wye) or phase pair (delta).
      Array<Complex, 3> Y;             ///< Constant admittance/phase (wye) or phase pair (delta) (impedance loads).
      Array<Complex, 3> I;             ///< Constant current / phase (wye) or phase pair (delta).

      std::vector<int> linkTable;      ///< Table of links that connect to us (for population purposes).

      /// @name Load Components. 
      /** Refer to Garcia et al. IEEE Transactions on Power Systems, 15, 2000. */
      /// @{
      Array<double, 3> PL;             ///< Real power component of total bus load.
      Array<double, 3> QL;             ///< Reactive power component of total bus load.
      Array<double, 3> PG;             ///< Real power generation at generator bus.
      Array<double, 3> QG;             ///< Reactive power generation at generator bus.
      /// @}

      /// @name The Jacobian.
      /** Refer to Garcia et al. IEEE Transactions on Power Systems, 15, 2000. */
      /// @{
      Array<double, 3> JacobA;         ///< Element a in equation (37).
      Array<double, 3> JacobB;         ///< Element b in equation (38).
      Array<double, 3> JacobC;         ///< Element c in equation (39).
      Array<double, 3> JacobD;         ///< Element d in equation (40).
      /// @}

      unsigned int matLoc;             ///< Starting idx of object's place in all matrices/equations.
      double maxVoltError;             ///< Maximum voltage error specified for node.
   };
   typedef struct NrBusData NrBusData;

   struct NrBranchData
   {
      Complex *Yfrom;                  ///< Branch admittance of from side of link.
      Complex *Yto;                    ///< Branch admittance of to side of link.
      Complex *YSfrom;                 ///< Self admittance seen on from side.
      Complex *YSto;                   ///< Self admittance seen on to side.
      unsigned char phases;            ///< Phases property.
      int from;                        ///< Index into bus data.
      int to;                          ///< Index into bus data.
   };
   typedef struct NrBranchData NrBranchData;

   struct YNr {
      int rowInd;                      ///< row loc of the element in 6n*6n Y matrix in NR solver.
      int colInd;                      ///< col location of the element in 6n*6n Y matrix in NR solver.
      double YValue;                   ///< value of the element in 6n*6n Y matrix in NR solver.
   };
   typedef struct YNr YNr;

   /// Bus Admittance structure.
   struct BusAdmit {
      int rowInd;                      ///< Row loc of the element in n*n bus admittance matrix in NR solver.
      int colInd;                      ///< Col loc of the element in n*n bus admittance matrix in NR solver.
      Matrix<Complex, 3, 3> Y;         ///< Complex value of elements in bus admittance matrix in NR solver.
      char size;                       ///< Size of the admittance diagonal - might be less than 3.
   };
   typedef struct BusAdmit BusAdmit;

   struct NrSolverVars {
      double *aLU;
      double *rhsLU;
      int *colsLU;
      int *rowsLU;
   };
   typedef struct NrSolverVars NrSolverVars;

   int solver_nr(unsigned int bus_count, NrBusData *bus, unsigned int branch_count, NrBranchData *branch,
                 bool *bad_computations);

}

#endif // SOLVER_NR_DOT_H
