#ifndef SOLVER_NR_DOT_H
#define SOLVER_NR_DOT_H

#include "complex.h"
#include "object.h"

struct  BUSDATA 
{
   /// bus type (0=PQ, 1=PV, 2=SWING).
   int type;
   /// Phases property.
   /** Used for construction of matrices (skip bad entries)
    *  [Split Phase | House present | To side of SPCT | Diff Phase Child |
    *   D | A | B | C] */
   unsigned char phases;   
   /// Original phases property.
   /** Follows same format.
    *  Used to save what object's original capabilities. */
   unsigned char origphases;  
   /// Bus voltage.
   complex *V;
   /// Constant power.
   complex *S;
   /// Constant admittance (impedance loads).
   complex *Y;
   /// Constant current.
   complex *I;
   /// Extra variable.
   /** Used mainly for current12 in triplex and "differently-connected"
    *  children. */
   complex *extra_var;
   /// Extra variable.
   /** Used mainly for nominal house current. */
   complex *house_var;
   /// Table of links that connect to us (for population purposes).
   int *Link_Table;
   /// Number of entries in the link table (number of links connected to us)
   unsigned int Link_Table_Size;
   /// Real power component of total bus load
   double PL[3];
   /// Reactive power component of total bus load
   double QL[3];
   /// Real power generation at generator bus
   double PG[3];
   /// Reactive power generation at generator bus
   double QG[3];
   /// kV basis
   double kv_base;
   /// MVA basis
   double mva_base;
   /// Element a in equation (37), which is used to update the Jacobian matrix at each iteration
   double Jacob_A[3];
   /// Element b in equation (38), which is used to update the Jacobian matrix at each iteration
   double Jacob_B[3];
   /// Element c in equation (39), which is used to update the Jacobian matrix at each iteration
   double Jacob_C[3];
   /// Element d in equation (40), which is used to update the Jacobian matrix at each iteration
   double Jacob_D[3];
   /// Starting index of this object's place in all matrices/equations
   unsigned int Matrix_Loc;
   /// Maximum voltage error specified for that node
   double max_volt_error;
   /// Original name
   char *name;
   /// Link to original object header
   OBJECT *obj;
   /// Index to parent node in BUSDATA structure - restoration related - may not be valid for meshed systems or reverse flow (restoration usage)
   int Parent_Node;
   /// Index to child nodes in BUSDATA structure - restoration related - may not be valid for meshed systems or reverse flow (restoration usage)
   int *Child_Nodes;
   /// Indexing variable to know location of next valid Child_Nodes entry
   unsigned int Child_Node_idx;
};
typedef struct BUSDATA BUSDATA;

struct BRANCHDATA
{
   /// Branch admittance of from side of link.
   complex *Yfrom;
   /// Branch admittance of to side of link.
   complex *Yto;
   /// Self admittance seen on from side.
   complex *YSfrom;
   /// Self admittance seen on to side.
   complex *YSto;
   /// Phases property. 
   /** Used for construction of matrices. */
   unsigned char phases;
   /// Original phases property.
   /** Follows same format.
    *  Used to save what object's original capabilities. */
   unsigned char origphases;
   /// Flags for induced faults.
   /** Used to prevent restoration of objects that should otherwise still
    *  be broken. */
   unsigned char faultphases;
   /// Index into bus data.
   int from;
   /// Index into bus data.
   int to;
   /// Index indicating next faulted link object below the current link object.
   int fault_link_below;
   /// Status of the object, if it is a switch (restoration module usage).
   bool *status;
   /// Type of link the object is.
   /** 0 = UG/OH line, 1 = Triplex line, 2 = switch, 3 = fuse, 
    *  4 = transformer, 5 = sectionalizer, 6 = recloser. */
   unsigned char lnk_type;
   /// Voltage ratio (V_from/V_to).
   double v_ratio;
   /// Original name.
   char *name;
   /// Link to original object header.
   OBJECT *obj;
   /// 3 phase fault currents on the from side.
   complex *If_from;
   /// 3 phase fault currents on the to side .
   complex *If_to;
} 
typedef struct BRANCHDATA BRANCHDATA;

struct Y_NR {
   /// row location of the element in 6n*6n Y matrix in NR solver.
   int row_ind;
   /// collumn location of the element in 6n*6n Y matrix in NR solver
   int   col_ind;
   /// value of the element in 6n*6n Y matrix in NR solver
   double Y_value;
}
typedef struct Y_NR Y_NR;

struct Bus_admit {
   /// Row location of the element in n*n bus admittance matrix in NR solver.
   int row_ind;
   /// Column location of the element in n*n bus admittance matrix in NR solver.
   int   col_ind;
   /// Complex value of elements in bus admittance matrix in NR solver.
   complex Y[3][3];
   /// Size of the admittance diagonal.
   /** Assumed square, useful for smaller size. */
   char size;
};
typedef struct Bus_admit Bus_admit;

struct NR_SOLVER_VARS {
   double *a_LU;
   double *rhs_LU;
   int *cols_LU;
   int *rows_LU;
};
typedef struct NR_SOLVER_VARS NR_SOLVER_VARS;

int64 solver_nr(unsigned int bus_count, BUSDATA *bus,
                unsigned int branch_count, BRANCHDATA *branch,
                bool *bad_computations);

#endif // SOLVER_NR_DOT_H
