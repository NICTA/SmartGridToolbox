#ifndef POWERFLOW_DOT_H
#define POWERFLOW_DOT_H

namespace SmartGridToolbox
{

   enum class BusType : int
   {
      PQ = 0,
      PV = 1,
      SL = 2
   };

#ifdef _POWERFLOW_CPP
#define GLOBAL
#define INIT(A) = (A)
#else
#define GLOBAL extern
#define INIT(A)
#endif

   namespace SmartGridToolbox
   {

      GLOBAL unsigned int NR_bus_count INIT(0);          ///< Newton-Raphson bus count used for determining size of bus vect 
      GLOBAL unsigned int NR_branch_count INIT(0);       ///< Newton-Raphson branch count
      /**< Used for determining size of branch vector */
      GLOBAL BUSDATA *NR_busdata INIT(NULL);             ///< Newton-Raphson bus data pointer array 
      GLOBAL BRANCHDATA *NR_branchdata INIT(NULL);       ///< Newton-Raphson branch data pointer array 
      GLOBAL int NR_curr_bus INIT(-1);                   ///< Newton-Raphson current bus indicator
      /**< Used to populate NR_busdata */
      GLOBAL int NR_curr_branch INIT(-1);                ///< Newton-Raphson current branch indicator
      /**< Used to populate NR_branchdata */
      GLOBAL bool NR_cycle INIT(true);                   ///< Newton-Raphson pass indicator
      /**< false = solution pass, true = metering/accumulation pass */
      GLOBAL bool NR_admit_change INIT(true);            ///< Newton-Raphson admittance matrix change detector.
      /**< Used to prevent recalculation of admit at every timestep */

      /* used by many powerflow enums */
#define UNKNOWN 0
#define ROUNDOFF 1e-6 //  Numerical accuracy for zero in float comparisons

   }
}

#endif // POWERFLOW_DOT_H
