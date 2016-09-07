//
//  var_.cpp
//  PowerTools++
//
//  Created by Hassan Hijazi on 20/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <string>
#include "PowerTools++/var.h"
#include <PowerTools++/Constraint.h>

using namespace std;

var_::var_():var_(NULL, -1){};
var_::var_(string name):var_(NULL, name){};
var_::var_(int idx):var_(NULL, idx){};
var_::var_(Model* model, string name):var_(model, name, -1){};
var_::var_(Model* model, int idx):_model(model), _idx(idx), _type(longreal), _dval(0){};
var_::var_(Model* model, string name, int idx){
    _name = name;
    _model = model;
    _idx = idx;
    _type = longreal;
    _dval = 0;
};

var_& var_::operator=(const var_& v){
    _model = v._model;
    _idx = v._idx;
    _name = v._name;
    _type = v._type;
    _dval = v._dval;
    _bounded_down = v._bounded_down;
    _bounded_up = v._bounded_up;
    for (auto c:v._cstrs){
        addConstraint(c.second);
    }
    for (auto id:v._hess){
        _hess.insert(id);
    }
    return *this;
}

/* Destructor */
var_::~var_(){
};

/* Accessors */

bool var_::is_bounded_above() const{
    return _bounded_up;
};

bool var_::is_bounded_below() const{
    return _bounded_down;
};

bool var_::is_constant() const{
    return (_type==constant);
};


bool var_::is_int() const{
    return (_type==integ);
};

bool var_::is_binary() const{
    return (_type==binary);
};

bool var_::is_real() const{
    return (_type==real);
};

bool var_::is_longreal() const{
    return (_type==longreal);
};


int var_::get_idx() const{
    return _idx;
};

VarType var_::get_type() const{
    return _type;
};

Constraint* var_::get_Constraint(int idx) const{
    return _cstrs.find(idx)->second;
};


/* Modifiers */
void var_::set_idx(int idx){
    _idx = idx;
};


bool var_::addConstraint(Constraint* c){
    if(_cstrs.insert(std::pair<int, Constraint*> (c->get_idx(),c)).second)
        return true;
    else
        cerr << "Cannot add constraint\n";
    exit(-1);
};

bool var_::delConstraint(Constraint* c){
    map<int,Constraint*>::iterator it = _cstrs.find(c->get_idx());
    if(it==_cstrs.end())
        return false;
    else
        _cstrs.erase(it);
    return true;
};



bool var_::addObjective(Objective* obj){
    return true;
};
bool var_::delObjective(Objective* obj){
    return true;
};

/* Operators */
Quadratic operator+(double cst, var_& v){
    Quadratic res(v);
    return res+=cst;
};

Quadratic operator-(double cst, var_& v){
    return -1*v+cst;
};

Quadratic operator*(double cst, var_& v){
    return Quadratic(v)*=cst;
};

Quadratic operator/(double cst, var_& v){
    return Quadratic(v)*=1/cst;
};

Quadratic operator+(var_& v1, var_& v2){
    Quadratic res(v1);
    return res+=v2;
};

Quadratic operator-(var_& v1, var_& v2){
    Quadratic res;
    res += v1;
    return res-=v2;
};

Quadratic operator*(var_& v1, var_& v2){
    return Quadratic(v1)*=v2;
};

Function operator/(var_& v1, var_& v2){
    return Function(v1)/=v2;
};

Function sin(var_& v){
    return sin(Function(v));
};


//Function var_::operator^(int p){
//    Function res(*this);
//    if(p==2) {
//        res *= res;
//        return res;
//    }
//    res = res^p;
//    return res;
//}

Quadratic var_::operator^(int p){
    assert(p==2);
    Quadratic res(*this);
    Quadratic res_(*this);
    return res*=res_;
}

/* Output */
void var_::print() const {
        cout << _name;
};


void var_::print_type() const {
    cout << "var" << _idx << ": ";
    if(is_binary())
        cout << "binary";
    if(is_real())
        cout << "real";
    if(is_int())
        cout << "integer";
    
};
