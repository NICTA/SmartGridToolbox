/** $Id: powerflow.h 1182 2008-12-22 22:08:36Z dchassin $
	Copyright (C) 2008 Battelle Memorial Institute
	@file powerflow.h
	@ingroup powerflow

 @{
 **/

#ifndef _POWERFLOW_H
#define _POWERFLOW_H

#include "solver_nr.h"

#ifdef _DEBUG
void print_matrix(complex mat[3][3]);
#endif

#ifdef _POWERFLOW_CPP
#define GLOBAL
#define INIT(A) = (A)
#else
#define GLOBAL extern
#define INIT(A)
#endif

GLOBAL unsigned int NR_bus_count INIT(0);          ///< Newton-Raphson bus count used for determining size of bus vect 
GLOBAL unsigned int NR_branch_count INIT(0);       ///< Newton-Raphson branch count
                                                   /**< Used for determining size of branch vector */
GLOBAL BUSDATA *NR_busdata INIT(NULL);             ///< Newton-Raphson bus data pointer array 
GLOBAL BRANCHDATA *NR_branchdata INIT(NULL);       ///< Newton-Raphson branch data pointer array 
GLOBAL double *deltaI_NR INIT(NULL);               ///< Newton-Raphson current differences pointer array 
GLOBAL int NR_curr_bus INIT(-1);                   ///< Newton-Raphson current bus indicator
                                                   /**< Used to populate NR_busdata */
GLOBAL int NR_curr_branch INIT(-1);                ///< Newton-Raphson current branch indicator
                                                   /**< Used to populate NR_branchdata */
GLOBAL int64 NR_iteration_limit INIT(500);         ///< Newton-Raphson iteration limit (per GridLAB-D iteration) 
GLOBAL bool NR_cycle INIT(true);                   ///< Newton-Raphson pass indicator
                                                   /**< false = solution pass, true = metering/accumulation pass */
GLOBAL bool NR_admit_change INIT(true);            ///< Newton-Raphson admittance matrix change detector.
                                                   /**< Used to prevent recalculation of admit at every timestep */
GLOBAL int NR_superLU_procs INIT(1);               ///< Newton-Raphson related - superLU MT processor count to request
                                                   /**< separate from thread_count */

/* used by many powerflow enums */
#define UNKNOWN 0
#define ROUNDOFF 1e-6 //  Numerical accuracy for zero in float comparisons

#include "powerflow_object.h"

#endif // _POWERFLOW_H

/**@} */
