//
//  Model.h
//  PowerTools++
//
//  Created by Hassan on 5/01/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#ifndef __PowerTools____Model__
#define __PowerTools____Model__

#include <stdio.h>
#include <PowerTools++/Type.h>
#include <PowerTools++/var_.h>
#include <PowerTools++/Constraint.h>
#include <PowerTools++/meta_Constraint.h>
#include <map>
#include <unordered_set>
#include <math.h>
#include <vector>
#include <coin/IpIpoptApplication.hpp>
#include <coin/IpTNLP.hpp>
#include <thread>
#include <gurobi_c++.h>

class Model {
    
protected:
    string*                         _name;
    vector<Constraint*>             _cons; /**< Sorted map (increasing index) pointing to all constraints contained in this model */
    map<int, meta_Constraint*>      _meta_cons; /**< Sorted map (increasing index) pointing to all meta-constraints contained in this model */
    vector<shared_ptr<Function>>               _functions;
    vector<shared_ptr<Quadratic>>               _quadratics;
    int                             _idx_var; /* Index counter for variables */
    int                             _idx_con; /* Index counter for variables */
public:
    int                             _nnz_g; /* Number of non zeros in the Jacobian */
    int                             _nnz_h; /* Number of non zeros in the Hessian */

    vector<var_*>                   _vars; /**< Sorted map (increasing index) pointing to all variables contained in this model */
    map<int, var_*>                 _meta_vars; /**< Sorted map (increasing index) pointing to all meta-variables contained in this model */
    map<std::string, int>           _hess_index; /**< Mapping variables to hessian index */
    Function*                       _obj; /** Objective function */
    double                          _opt;
    ObjectiveType                   _objt; /** Minimize or maximize */
    /** Constructor */
    //@{
    Model();
    //@}
    
    /* Destructor */
    ~Model();
    
    /* Accessors */

    friend int get_hess_index(var_* vi, var_* vj);
    friend int get_hess_index(var_* vi, int vjd);
    
    int get_nb_vars() const;
    
    int get_nb_cons() const;
    
    int get_nb_nnz_g() const;
    
    int get_nb_nnz_h() const;
    
    bool hasVar(var_* v) const;
    
//    int get_hess_index(int vid, int vjd) const;
    
    template<typename Number> var<Number>* getVar_(int idx) const;
    
    const vector<Constraint*>& get_cons() const;
    
    /* Modifiers */
    
    void addVar(var_& v);
    void addMetaVar(var_& v);
    void addMetaConstant(var_& c);
    void update_hess_link(var_* v);
    void update_hess_link();    
    var_* getVar(int vid);
    void delVar(var_* v);
    void addConstraint(Constraint c);
    void addConstraint(Constraint* c);
    void on_off(Constraint c, var<bool>& on);
    void on_off(var<>& v, var<bool>& on);
    void add_on_off_McCormick(std::string name, var<>& v, var<>& v1, var<>& v2, var<bool>& on);
    void add_McCormick(std::string name, var<>& v, var<>& v1, var<>& v2);

    int has_function(shared_ptr<Function> f);
    void init_functions(int size);
    void add_functions(shared_ptr<Function> f);
    void addMetaConstraint(meta_Constraint& c);
    void concretise(meta_Constraint& mc, int meta_link, string name);
    void delConstraint(Constraint* c);
    void setObjective(Function* f);
    void setObjectiveType(ObjectiveType);
    void check_feasible(const double* x);
    void fill_in_var_bounds(double* x_l ,double* x_u);
    void fill_in_var_init(double* x);
    void fill_in_cstr_bounds(double* g_l ,double* g_u);
    void fill_in_obj(const double* x , double& res);
    void fill_in_grad_obj(const double* x , double* res);
    void fill_in_cstr(const double* x , double* res, bool new_x);
    void fill_in_jac(const double* x , double* res, bool new_x);
    void fill_in_jac_nnz(int* iRow , int* jCol);
    void fill_in_hess(const double* x , double obj_factor, const double* lambda, double* res, bool new_x);
    void eval_funcs_parallel(const double* x , int start, int end);
    void fill_in_hess1(const double* x , double obj_factor, const double* lambda, double* res);
    void fill_in_hess2(const double* x , double obj_factor, const double* lambda, double* res);
    void fill_in_hess_multithread(const double* x , double obj_factor, const double* lambda, double* res, int start, int end);
    void multi_thread_hess(std::map<int, Constraint*>* cons, const double* x , const double* lambda, double* res, int start, int end);
    void fill_in_hess_nnz(int* iRow , int* jCol);
    void fill_in_var_linearity(Ipopt::TNLP::LinearityType* var_types);
    void fill_in_cstr_linearity(Ipopt::TNLP::LinearityType* const_types);
    void solve();
    friend std::vector<int> bounds(int parts, int mem);
//    IpoptProgram* create_ipopt_program();
    /* Operators */
    
    //    Function& operator+(const Function& f);
    
    
    /* Output */
    void print_functions() const;
    void print() const;
    void print_solution() const;
    
    
};



#endif /* defined(__PowerTools____Model__) */
