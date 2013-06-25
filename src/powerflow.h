/** $Id: powerflow.h 1182 2008-12-22 22:08:36Z dchassin $
	Copyright (C) 2008 Battelle Memorial Institute
	@file powerflow.h
	@ingroup powerflow

 @{
 **/

#ifndef _POWERFLOW_H
#define _POWERFLOW_H

#include "gridlabd.h"
#include "solver_nr.h"

#ifdef _POWERFLOW_CPP
#define
#define INIT(A) = (A)
#else
#define extern
#define INIT(A)
#endif

#ifdef _DEBUG
void print_matrix(complex mat[3][3]);
#endif

bool use_line_cap INIT(false);         /**< Flag to include line capacitance quantities */
bool use_link_limits INIT(true);       /**< Flag to include line/transformer ratings and provide a warning if exceeded */
MATRIXSOLVERMETHOD matrix_solver_method INIT(MM_SUPERLU);   /**< Newton-Raphson uses superLU as the default solver */
unsigned int NR_bus_count INIT(0);     /**< Newton-Raphson bus count - used for determining size of bus vector */
unsigned int NR_branch_count INIT(0);  /**< Newton-Raphson branch count - used for determining size of branch vector */
BUSDATA *NR_busdata INIT(NULL);        /**< Newton-Raphson bus data pointer array */
BRANCHDATA *NR_branchdata INIT(NULL);  /**< Newton-Raphson branch data pointer array */
double *deltaI_NR INIT(NULL);          /**< Newton-Raphson current differences pointer array */
int NR_curr_bus INIT(-1);              /**< Newton-Raphson current bus indicator - used to populate NR_busdata */
int NR_curr_branch INIT(-1);           /**< Newton-Raphson current branch indicator - used to populate NR_branchdata */
int64 NR_iteration_limit INIT(500);    /**< Newton-Raphson iteration limit (per GridLAB-D iteration) */
bool NR_cycle INIT(true);              /**< Newton-Raphson pass indicator 
                                        *   false = solution pass, true = metering/accumulation pass */
bool NR_admit_change INIT(true);       /**< Newton-Raphson admittance matrix change detector.
                                        *   Used to prevent complete recalculation of admittance at every timestep */
int NR_superLU_procs INIT(1);          /**< Newton-Raphson related - superLU MT processor count to request
                                        *   separate from thread_count */
TIMESTAMP NR_retval INIT(TS_NEVER);    /**< Newton-Raphson current return value
                                        *   if t0 objects know we aren't going anywhere */
OBJECT *NR_swing_bus INIT(NULL);       /**< Newton-Raphson swing bus */
bool FBS_swing_set INIT(false);        /**< Forward-Back Sweep swing assignment variable */
bool show_matrix_values INIT(false);   /**< flag to enable dumping matrix calculations as they occur */
double primary_voltage_ratio INIT(60.0);  /**< primary voltage ratio
                                           *   (@todo explain primary_voltage_ratio in powerflow (ticket #131) */
double nominal_frequency INIT(60.0);   /**< nomimal operating frequencty */
double warning_underfrequency INIT(55.0); /**< frequency below which a warning is posted */
double warning_overfrequency INIT(65.0);  /**< frequency above which a warning is posted */
double nominal_voltage INIT(240.0);    /**< nominal voltage level */
double warning_undervoltage INIT(0.8); /**< voltage magnitude (per unit) below which a warning is posted */
double warning_overvoltage INIT(1.2);  /**< voltage magnitude (per unit) above which a warning is posted */
double warning_voltageangle INIT(2.0); /**< voltage angle (over link) above which a warning is posted */
bool require_voltage_control INIT(false); /**< flag to enable voltage control source requirement */
double geographic_degree INIT(0.0);    /**< topological degree factor */
complex fault_Z INIT(complex(1e-6,0)); /**< fault impedance */
double default_maximum_voltage_error INIT(1e-6);   /**< default sync voltage convergence limit [puV] */
double default_maximum_power_error INIT(0.0001);   /**< default power convergence limit for multirun */
OBJECT *restoration_object INIT(NULL); /**< restoration object of the system */
OBJECT *fault_check_object INIT(NULL); /**< fault_check object of the system */

/* used by many powerflow enums */
#define UNKNOWN 0
#define ROUNDOFF 1e-6                                      /  Numerical accuracy for zero in float comparisons


#include "powerflow_object.h"

#endif                                                     /  _POWERFLOW_H

/**@}*/
