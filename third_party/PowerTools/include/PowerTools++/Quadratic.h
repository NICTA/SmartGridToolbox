//
//  Quadratic.h
//  PowerTools++
//
//  Created by Hassan on 9/02/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#ifndef __PowerTools____Quadratic__
#define __PowerTools____Quadratic__

#include <stdio.h>
#include <math.h>
#include <map>
#include <vector>
#include <assert.h>
#include <iostream>
#include <PowerTools++/var_.h>
#include "meta_var.h"
#include "meta_constant.h"

using namespace std;

/** A Quadratic Function has the form: x^T.Q.x + a^T.x + b */
class Quadratic{
public:
    map<int, double>                            _coefs; /**< Linear part of a function, map linking variable id to coefficient */
    map<int, map<int, double>*>                 _qmatrix; /**< Quadratic part of the function, Q matrix in x'Qx */
    double                                      _cst; /**< Constant part of the function*/
    map<int, var_*>                             _lin_vars; /**< Sorted map pointing to all variables contained in the linear part of this function */
    map<int, var_*>                             _quad_vars; /**< Sorted map pointing to all variables contained in the quadratic part of this function */
    map<int, std::set<int>*>                    _hess; /* Hessian links between variables */
    
    Quadratic();
    Quadratic(const Quadratic& q);
    Quadratic(var_& v);
    Quadratic(double cst);
    ~Quadratic();
    
    /* Boolean Requests */
    bool is_constant() const;
    bool is_linear() const;
    bool is_quadratic() const;
    bool has_q_var(int vid) const; /**< Checks if that variable index appears in the Q matrix */
    bool has_var(int vid) const; /**< Checks if that variable index appears in the Q matrix */
    
    /* Accessors */
    var_* get_var(int vid) const;
    int get_nb_vars() const;
    double get_coeff(int vid) const;
    double get_const()  const;
    double get_q_coeff(int vid, int vjd)  const;
    double eval(const double* x) const;
    double eval_new() const;
    double eval_meta(const double* x, map<int, double>& meta_cons, map<int, var_*>& meta_vars) const;
    double eval_dfdx(int vid, const double* x) const;
    double eval_dfdxdy(int vid, int vjd) const; // Constant
    int get_q_nnz() const; /**< Number of non zeros in Q */
    
    /* Modifiers */
    void add(double cst, var_* v); /**< add (cst*vid) */
    void merge_vars(const Quadratic& q);
    void update_q_vars(int vid); /**< Remove variable if it does not appear in Q */
    void add_hess_link(int vid, int vjd);
    void remove_hess_link(int vid, int vjd);
//    void delete_vars(); /**< Clear linear and quadratic variables */
    
    /* Operators */
    bool operator==(const Quadratic& q) const;
    bool operator!=(const Quadratic& q) const;
    Quadratic& operator+=(double cst);
    Quadratic& operator-=(double cst);
    Quadratic& operator*=(double cst);
    Quadratic& operator+=(var_& v);
    Quadratic& operator-=(var_& v);
    Quadratic& operator*=(var_& v);
    Quadratic& operator+=(const Quadratic& q);
    Quadratic& operator-=(const Quadratic& q);
    Quadratic& operator*=(const Quadratic& q);

    friend Quadratic operator+(double cst, Quadratic q);
    friend Quadratic operator-(double cst, Quadratic q);
    friend Quadratic operator*(double cst, Quadratic q);
    friend Function operator/(double cst, Quadratic q);

    friend Quadratic operator+(Quadratic q, double cst);
    friend Quadratic operator-(Quadratic q, double cst);
    friend Quadratic operator*(Quadratic q, double cst);
    friend Quadratic operator/(Quadratic q, double cst);
    
    friend Quadratic operator+(Quadratic q, var_& v);
    friend Quadratic operator-(Quadratic q, var_& v);
    friend Function operator*(Quadratic q, var_& v);
    friend Function operator/(Quadratic q, var_& v);
    
    friend Quadratic operator+(var_& v, Quadratic q);
    friend Quadratic operator-(var_& v, Quadratic q);
    friend Function operator*(var_& v, Quadratic q);
    friend Function operator/(var_& v, Quadratic q);

    
    friend Quadratic operator+(Quadratic q1, const Quadratic& q2);
    friend Quadratic operator-(Quadratic q1, const Quadratic& q2);
    friend Function operator*(Quadratic q1, const Quadratic& q2);
    friend Function operator/(Quadratic q1, const Quadratic& q2);

    friend Function cos(const Quadratic& q);
    friend Function sin(const Quadratic& q);
    friend Function sqrt(const Quadratic& q);
    
    Quadratic concretise();
    
    /* Derivatives */
    Quadratic get_dfdx(var_* v); // Linear/Constant
    
    /* Memory */
    void reset_coeffs(); /**< Clear linear and quadratic coefficients */
    
    /* Output */
    void print() const;
};

#endif /* defined(__PowerTools____Quadratic__) */
