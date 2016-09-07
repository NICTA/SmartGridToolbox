//
//  Function.h
//  PowerTools++
//
//  Created by Hassan on 19/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef __PowerTools____Function__
#define __PowerTools____Function__
#include <PowerTools++/Type.h>
#include <PowerTools++/var.h>
#include <PowerTools++/Quadratic.h>
#include <memory>
#include <math.h>
#include <map>
#include <vector>
#include <assert.h>
#include <iostream>

using namespace std;
/** A Function has the form: coeff*(_lparent _otype _rparent) + x^T.Q.x + a^T.x + b */
class Function {
    
protected:

    int                                   _idx; /**< Function index */
    OperatorType                          _otype; /**< Operator type in expression tree */
    Quadratic                             _quad; /**< Quadratic part in Function */
    double                                _coeff; /**< Coefficient in front of Function */
    
public:
    shared_ptr<Function>                  _lparent; /**< Left parent in expression tree */
    shared_ptr<Function>                  _rparent; /**< Right parent in expression tree */
    Functiontype                          _ftype; /**< Function type: linear, quadratic or nonlinear */
    vector<double>                                _val; /**< Current value of f, from last call to evaluate() */
    vector<bool>                                  _evaluated; /**< True if already called evaluate() */
    std::string                           _name;    
    map<int, shared_ptr<Function>>        _dfdx; /**< Partial derivatives of f */
    map<int, var_*>                       _vars; /**< Sorted map pointing to all variables contained in this function */
    map<int, unique_ptr<set<int>>>              _hess; /* Hessian links between variables */
    /** Constructor */
    //@{
    Function();
    Function(double cst);
    Function(int cst);
    Function(var_& v);
    Function(const Quadratic& q);
    Function(const Function& lparent, OperatorType otype, const Function& rparent);
    Function(const Function& f);
    Function(std::string name);
    Function(std::string name, int idx);
    void clone(const Function* f);
    void cloneTree(const Function& f);
    void quad_clone(const Function* f); /**< only clone the quadratic part */
    //@}
    
    /* Destructor */
    ~Function();
    
    
    /* Accessors */
    Functiontype get_type(){return _ftype;};
    int get_idx() const;
    var_* get_var(int vid) const;
    double get_const() const;
    size_t get_nb_vars() const;
    double get_coeff(int vid) const;
    template<typename Number> var<Number>* get_var_(int idx) const;
    double get_q_coeff(int vid1, int vid2) const;
    double eval(const double* x) const;
    double eval_meta(const double *x, map<int, double>& meta_coeff, map<int, var_*>& meta_vars, int meta_link);
    double eval_dfdx(int vid, const double* x) const;
    shared_ptr<Function> getTree() const; /**< returns coeff*(_lparent _otype _rparent) */
    const Quadratic* get_quad() const;
    
    /* Boolean Requests */
    bool same(const double* x) const; /**< if the function variables changed their values */
    bool has_var(int vid) const;
    bool has_q_var(int vid) const;
    bool has_dfdx(int vid) const;
    bool is_nonlinear() const;
    bool is_quadratic() const;
    bool is_linear() const;
    bool is_constant() const;
    bool is_leaf() const; //constant, linear or quadratic
    bool symmetric(const Function& f) const;
    
    
    /* Modifiers */
    void set_idx(int idx);
//    void collapse_expr(); /**< transform the original form: coeff*(_lparent _otype _rparent) + x^T.Q.x + a^T.x + b into 1*(_lparent1 _otype _rparent1) + 0 by putting the quadratic term in the expression tree */
    bool setQCoeff(int vid1, int vid2, double v);
    void set_dfdx(int vid, shared_ptr<Function> f);
    void add_hess_link(int vid, int vjd);
    void update_type();
    void reset();
    void resetTree();
    
    /* Operators */
    void merge_dfdx(const Function& f1);
    void merge_vars(const Function& f1);
    void merge_vars(const Quadratic& q);
    void compute_dfdx(var_* v);
    void full_hess();
    void full_hess(const Function& f);
    void refactor(); /**< Update the expression tree by separating the nonlinear part from the quadratic part */
    
    Function outer_approx(const double* x) const;
    
    bool operator==(const Function& f) const;
    bool operator!=(const Function& f) const;

    Function& operator+=(double cst);
    Function& operator-=(double cst);
    Function& operator*=(double cst);
    
    Function& operator+=(var_& v);
    Function& operator-=(var_& v);
    Function& operator*=(var_& v);
    
    Function& operator+=(const Quadratic& f);
    Function& operator-=(const Quadratic& f);
    Function& operator*=(const Quadratic& f);
    
    Function& operator+=(const Function& f);
    Function& operator-=(const Function& f);
    Function& operator*=(const Function& f);
    Function& operator/=(const Function& f);

    
    friend Function operator+(Function f1, const Function& f2);
    friend Function operator-(Function f1, const Function& f2);
    friend Function operator*(Function f1, const Function& f2);
    friend Function operator/(Function f1, const Function& f2);

    friend Function operator+(Function f1, double cst);
    friend Function operator-(Function f1, double cst);
    friend Function operator*(Function f1, double cst);
    friend Function operator/(Function f1, double cst);

    friend Function operator+(double cst, Function f1);
    friend Function operator-(double cst, Function f1);
    friend Function operator*(double cst, Function f1);
    friend Function operator/(double cst, Function f1);
    
    friend Function operator^(Function f, int p);
    friend Function cos(Function& f);
    friend Function sin(Function& f);
    friend Function sqrt(Function& f);
    friend Function expo(Function& f);
    friend Function log(Function& f);

    friend Function cos(Function&& f);
    friend Function sin(Function&& f);
    friend Function sqrt(Function&& f);
    friend Function expo(Function&& f);
    friend Function log(Function&& f);

    Function& operator=(const Function& f);
    
    Function concretise();
    
    /* Output */
    void print_domain() const;
    void print(bool domain) const;
    void print_expr(bool brackets) const;
    void print_linear() const;
    void print_quad() const;

    Function quad_ch_right(var<>& x, var<>& z, var<bool>& u, double* vals) const;

    Function quad_ch_left(var<>& x, var<>& z, var<bool> &u, double* vals) const;
};


//-(double)              getHess: (MPVar*) vi vj:(MPVar*) vj;
//-(ORInt)               nnz; /* Number of non zeros in Q */
//-(ORFloat)             cst;
//-(void)                add: (ORFloat) cst;
//-(void)                add: (ORFloat) coef times: (MPVar*) var; /* coef.var */
//-(void)                add: (ORFloat) coef times_square: (MPVar*) var; /* coef.var^2 */
//-(void)                add: (ORFloat) coef times: (MPVar*) var1 product: (MPVar*) var2; /* coef.var1.var2 */
//-(double)              eval: (double*) x;
//-(void)                eval_grad: (double*) x grad: (double*) grad;


#endif /* defined(__PowerTools____Function__) */
