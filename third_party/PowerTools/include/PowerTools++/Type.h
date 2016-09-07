//
//  Type.h
//  PowerTools++
//
//  Created by Hassan on 16/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef PowerTools___Type_h
#define PowerTools___Type_h
typedef enum { binary, integ, real, longreal, constant } VarType;
typedef enum { infeasible, optimal, suboptimal, unbounded, error } Outcome;
typedef enum { geq, leq, eq } ConstraintType;
typedef enum { constant_, lin_, quad_, nlin_ } Functiontype;  /* Function type in constraint: Constant, Linear, Quadratic or Nonlinear constraint */
typedef enum { minimize, maximize } ObjectiveType;
typedef enum { id_, plus_, minus_, product_, div_, power_, cos_, sin_, sqrt_, exp_, log_} OperatorType;  /* Operation type in the expression tree */
typedef enum { ipopt, gurobi, bonmin } SolverType;  /* Solver type */
#endif
