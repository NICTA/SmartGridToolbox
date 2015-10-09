//
//  var.h
//  PowerTools++
//
//  Created by Hassan on 16/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#ifndef PowerTools___var_h
#define PowerTools___var_h
#include <map>
#include <vector>
#include <list>
#include <typeinfo>
#include <limits>
#include <PowerTools++/Type.h>
#include <PowerTools++/var_.h>

class Model;
class Objective;
class Constraint;


template<typename Number = double>
class var :public var_{
    
protected:
    Number                      _val; /**< Variable current value */
    Number                      _lb; /**< Lower Bound */
    Number                      _ub; /**< Upper Bound */
    Number                      _lb_off; /**< Lower Bound when the variable is not active */
    Number                      _ub_off; /**< Upper Bound when the variable is not active */


public:
    /* Constructors */
    
    //@{
    /** Unbounded constructor */
    var();
    var(std::string name);
    var(int idx);
    var(Model* model, std::string name);
    var(Model* model, int idx);
    var(Model* model, std::string name, int idx);
    var(std::string name, int idx);    
    //@}
    
    /** Constructor with lower Bound only */
    var(Model* model, std::string name, int idx, Number lb);

    //@{
    /** Bounded variable constructor */
    var(std::string name, Number lb, Number ub);
    var(Model* model, std::string name, int idx, Number lb, Number ub);
    var(Model* model, std::string name, int idx, Number lb, Number ub, Number lb_off, Number ub_off);
    //@}

    /* Destructor */
    ~var();
    
    var& operator=(Number cst);
    
    /* Accessors */
    
    Number    get_lb() const;
    
    Number    get_ub() const;

    
    Number    get_lb_off() const;
    
    Number    get_ub_off() const;

    
    Number    get_value() const;
    
    /* Modifiers */
    void    init(std::string name, Number lb, Number ub);
    void    init(std::string name, Number lb, Number ub, Number lb_off, Number ub_off);
    void    init(std::string name);
    void    set_lb(Number lb);
    void    set_ub(Number ub);
    void    set_val(Number val);
    
    
    /* Output */
    void print() const;

    
};


#endif
