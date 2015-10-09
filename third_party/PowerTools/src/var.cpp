//
//  Var.cpp
//  PowerTools++
//
//  Created by Hassan on 16/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "PowerTools++/var.h"
#include "PowerTools++/Constraint.h"
using namespace std;


template<typename Number> var<Number>::var():var(NULL, -1){};
template<typename Number> var<Number>::var(string name):var(NULL, name){};
template<typename Number> var<Number>::var(int idx):var(NULL, idx){};
template<typename Number> var<Number>::var(Model* model, string name):var(model, name, -1){};
template<typename Number> var<Number>::var(Model* model, int idx):var(model, "noname", idx){};
template<typename Number> var<Number>::var(string name, int idx):var(NULL, name, idx){};
template<typename Number> var<Number>::var(Model* model, string name, int idx):var(model, name, idx, -numeric_limits<Number>::infinity(), numeric_limits<Number>::infinity()){};
//@}

/** Constructor with lower Bound only */
template<typename Number> var<Number>::var(Model* model, string name, int idx, Number lb):var(NULL, name, idx, lb, numeric_limits<Number>::infinity()){};

//@{
/** Bounded variable constructor */
template<typename Number> var<Number>::var(string name, Number lb, Number ub):var(NULL, name, -1, lb, ub){};
template<typename Number> var<Number>::var(Model* model, string name, int idx, Number lb, Number ub):var(model,name,idx,lb,ub,lb,ub){};
template<typename Number> var<Number>::var(Model* model, string name, int idx, Number lb, Number ub, Number lb_off, Number ub_off){
    _name = name;
    _model = model;
    _idx = idx;
    if(typeid(Number)==typeid(bool)){
        _type = binary;
        _lb = false;
        _ub = true;
        // WARNING setting bounds of a binary variable!
    }
    else{
        if(typeid(Number)==typeid(int))
            _type = integ;
        else{
           if(typeid(Number)==typeid(float))
               _type = real;
            else
               _type = longreal;
        }
    }
    _bounded_down = true;
    _bounded_up = true;
    if (lb==-numeric_limits<Number>::infinity()) {
        _bounded_down = false;
    }
    else
    if (ub==numeric_limits<Number>::infinity()) {
        _bounded_up = false;
    }
    _lb = lb;
    _ub = ub;
    _lb_off = lb_off;
    _ub_off = ub_off;
    _val = 0.0;
};
//@}

/* Destructor */
template<typename Number> var<Number>::~var(){};


template<typename Number>var<Number>& var<Number>::operator=(Number cst) {
    _val = cst;
    return *this;
};

/* Accessors */
template<typename Number>Number var<Number>::get_lb() const{
    return _lb;
};
template<typename Number>Number var<Number>::get_ub() const{
    return _ub;
};
template<typename Number>Number var<Number>::get_lb_off() const{
    return _lb_off;
};
template<typename Number>Number var<Number>::get_ub_off() const{
    return _ub_off;
};

template<typename Number>Number var<Number>::get_value() const{
    return _val;
};


/* Modifiers */

template<typename Number>void var<Number>::init(string name, Number lb, Number ub){
    init(name, lb, ub, lb, ub);
};


template<typename Number>void var<Number>::init(string name, Number lb, Number ub, Number lb_off, Number ub_off){
    init(name);
    _bounded_down = true;
    _bounded_up = true;
    if (lb==-numeric_limits<Number>::infinity()) {
        _bounded_down = false;
    }
    else
        if (ub==numeric_limits<Number>::infinity()) {
            _bounded_up = false;
        }
    _lb = lb;
    _ub = ub;
    _lb_off = lb_off;
    _ub_off = ub_off;
}


template<typename Number>void var<Number>::init(string name){
    _name = name;
    _model = NULL;
    _cstrs.clear();
    _hess.clear();
    _idx = -1;
    if(typeid(Number)==typeid(bool)){
        _type = binary;
        _lb = false;
        _ub = true;
    }
    else{
        if(typeid(Number)==typeid(int))
            _type = integ;
        else{
            if(typeid(Number)==typeid(float))
                _type = real;
            else
                _type = longreal;
        }
    }
    _bounded_down = false;
    _bounded_up = false;
    _lb=-numeric_limits<Number>::infinity();
    _ub=numeric_limits<Number>::infinity();
    _lb_off=0;
    _ub_off=0;
    _val = 0.0;
};

template<typename Number>void var<Number>::set_lb(Number lb){
    _lb = lb;
};
template<typename Number>void var<Number>::set_ub(Number ub){
    _ub = ub;
};
template<typename Number>void var<Number>::set_val(Number val){
    _val = val;
};


/* Output */
template<typename Number> void var<Number>::print () const{
    switch (_type) {
        case integ:
            cout << "Int ";
            break;
        case binary:
            cout << "Binary ";
            break;
            
        default:
            cout << "Real ";
            break;
    }
    cout << _name;
    if (!_bounded_down && !_bounded_down) {
        cout << " (unbounded) = ";
        printf("%.10f;\n", (double)_val);
//        cout << "in constraints: ";
//        for (auto it:_cstrs) {
//            it.second->print();
//        }
        return;
    }
    cout << " in ";
    switch (_type) {
        case integ:
            cout << "{" << get_lb() << "," << get_ub() << "}";
            cout << " = " << _val;
            break;
        case binary:
            cout << "{" << 0 << "," << 1 << "}";
            cout << " = " << _val;
            break;
        default:
            cout << "[" << get_lb() << "," << get_ub() << "]";
            printf(" = %.10f", (double)_val);
            break;
    }


    cout << ";" << endl;
//    cout << "in constraints: ";
//    for (auto it:_cstrs) {
//        it.second->print();
//    }
};


template class var<bool>;
template class var<int>;
template class var<float>;
template class var<>;
