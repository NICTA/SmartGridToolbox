//
//  vbase.h
//  PowerTools++
//
//  Created by Hassan on 20/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef PowerTools___vbase_h
#define PowerTools___vbase_h
#include <map>
#include <set>
#include <vector>
#include <list>
#include <typeinfo>
#include <limits>
#include <PowerTools++/Type.h>
#include <stdio.h>
#include <iostream>

class Model;
class Objective;
class Constraint;
class Function;
class Quadratic;
class Expression;
using namespace std;
class meta_var;
/** Backbone class for a variable */
class var_ {
protected:
    Model*                      _model; /**< Corresponding model */
    int                         _idx; /**< Variable index */
    VarType                     _type; /**< Variable type: real, binary or integer */

public:
    double                      _dval; /**< default value, default type is double. */
    bool                        _bounded_up;
    bool                        _bounded_down;
    vector<int>                         _meta_var;
    std::string                 _name;
    std::map<int, Constraint*>  _cstrs; /**< Sorted map pointing to all constraints containing this variable */
    std::set<int>               _hess;/**< Variables linked in the hessian */
    std::map<int,int>           _hess_id;/**< Hessian index for variables linked in second derivatives */

    /* Constructors */
    
        //@{
    /** Unbounded constructor */
    var_();
    var_(std::string name);
    var_(int idx);
    var_(Model* model, std::string name);
    var_(Model* model, int idx);
    var_(Model* model, std::string name, int idx);
        //@}
    
    
    /* Destructor */
    ~var_();
    
    /* Accessors */
    bool    is_bounded_above() const;
    bool    is_bounded_below() const;
    bool    is_constant() const;
    bool    is_int() const;
    bool    is_binary() const;
    bool    is_real() const;
    bool    is_longreal() const;
    int     get_idx() const;
    
    VarType     get_type() const;
    Constraint* get_Constraint(int idx) const;
    
    
    /* Modifiers */
    void    set_idx(int idx);
    bool    addConstraint(Constraint* c);
    bool    delConstraint(Constraint* c);
    bool    addObjective(Objective* obj);
    bool    delObjective(Objective* obj);
    
    /* Operators */
    friend Quadratic operator+(double cst, var_& v);
    friend Quadratic operator-(double cst, var_& v);
    friend Quadratic operator*(double cst, var_& v);
    friend Quadratic operator/(double cst, var_& v);
    
    friend Quadratic operator+(var_& v1, var_& v2);
    friend Quadratic operator-(var_& v1, var_& v2);
    friend Quadratic operator*(var_& v1, var_& v2);
    friend Function operator/(var_& v1, var_& v2);
    friend Function sin(var_& v);
    friend Function cos(var_& v);
    
    var_& operator=(const var_& v);
//    Function operator^(int p);
    Quadratic operator^(int p);

    /* Output */
    void print() const;
    void print_type() const;
};
#endif
