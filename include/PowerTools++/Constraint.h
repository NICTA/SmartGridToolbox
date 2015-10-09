//
//  Constraint.h
//  PowerTools++
//
//  Created by Hassan on 18/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef PowerTools___Constraint_h
#define PowerTools___Constraint_h

#include <PowerTools++/var.h>
#include <PowerTools++/Function.h>
class meta_Constraint;

class Constraint :public Function{
    
protected:
    Model*                      _model; /**< Corresponding model */
    
public:
    map<int, var_*>             _meta_vars; /**< map linking meta-variable id with concretised var */
    map<int, int>               _meta_ids; /**< map linking concretised var id with meta-variable id, i.e., (var,meta_var) */
    map<int, double>            _meta_coeff; /**< map linking meta_variable id with concretised value */
    meta_Constraint*            _meta_constr;/**< points to corresponding meta-constraint */
    int                         _meta_link;
    ConstraintType              _ctype; /**< Constraint type: leq, geq or eq */
    double _rhs;
    /** Constructor */
    //@{
    Constraint();
    Constraint(const Constraint& c);
    Constraint(std::string name);
    Constraint(int idx);
    Constraint(Model* model, std::string name);
    Constraint(Model* model, int idx);
    Constraint(Model* model, std::string name, int idx);
    Constraint(Model* model, std::string name, int idx, ConstraintType ctype);
    //@}
    
    Function* get_meta_dfdx(var_* v);
    double get_rhs() const;
    std::string get_name() const;
    
    /* Destructor */
    ~Constraint();
    
    
    /* Boolean Requests */
    bool has_meta() const;/**< returns true if constraint was build from a meta-constraint */
    
    /* Operators */
    Constraint& operator<=(double rhs);
    Constraint& operator>=(double rhs);
    Constraint& operator=(double rhs);
    Constraint& operator=(const Function& f);
//
//    Constraint& operator<=(int rhs);
//    Constraint& operator>=(int rhs);
//    Constraint& operator=(int rhs);

    /* Accessors */
    int get_idx() const;
    int get_type() const;
    int get_ftype() const;
    /* Modifiers */
    void set_idx(int idx);
    
    /* Output */
    void print();
    
    
};
#endif
