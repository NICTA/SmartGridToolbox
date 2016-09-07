//
//  Constraint.cpp
//  PowerTools++
//
//  Created by Hassan on 18/12/2014.
//  Copyright (c) 2014 NICTA. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <cstring>
#include "PowerTools++/Constraint.h"
#include "PowerTools++/meta_Constraint.h"

using namespace std;

/** Constructor */
//@{
Constraint::Constraint():Constraint(NULL, -1){};
Constraint::Constraint(string name):Constraint(NULL, name){};
Constraint::Constraint(int idx):Constraint(NULL, idx){};
Constraint::Constraint(Model* model, string name):Constraint(model, name, -1){};
Constraint::Constraint(Model* model, int idx):Constraint(model, "constr", idx){};
Constraint::Constraint(Model* model, string name, int idx):Constraint(model, name, idx, leq){};
Constraint::Constraint(Model* model, string name, int idx, ConstraintType ctype){
    _model = model;
    _name = name;
    _idx = idx;
    _ctype = ctype;
    _rhs = 0;
    _meta_constr = NULL;
    _meta_link = -2;
};

Constraint::Constraint(const Constraint& c){
    _model = c._model;
    _name = c._name;
    _idx = c._idx;
    _ctype = c._ctype;
    _ftype = c._ftype;
    _otype = c._otype;
    _rhs = c._rhs;
    _meta_constr = c._meta_constr;
    _meta_link = c._meta_link;
    for (auto &it:c._meta_coeff){
        _meta_coeff.insert(pair<int, double>(it.first, it.second));
    }
    for (auto &it:c._meta_vars){
        _meta_vars.insert(pair<int, var_*>(it.first, it.second));
    }
    for (auto &it:c._meta_ids){
        _meta_ids.insert(pair<int, int>(it.first, it.second));
    }
    for (auto &it:c._dfdx){
        _dfdx.insert(pair<int, shared_ptr<Function>>(it.first, it.second));
    }
    int vid = 0;
    for (auto &it:c._hess){
        vid = it.first;
        for(int vjd:*it.second)
        {
            add_hess_link(vid, vjd);
        }

    }
    _quad += c._quad;
    merge_vars(c);
}

//@}

/* Destructor */
Constraint::~Constraint(){};

double Constraint::get_rhs() const{
    return _rhs;
};

std::string Constraint::get_name() const{
    return _name;
};

bool Constraint::has_meta() const{/**< returns true if the constraint was build from a meta-constraint (calling a concretise() function) */
    return (_meta_constr);
}


Function* Constraint::get_meta_dfdx(var_* v){
//        return _meta_constr->_dfdx[v->_meta_var[get_idx()]].get();
    return _meta_constr->_dfdx[_meta_ids[v->get_idx()]].get();
}




Constraint& Constraint::operator<=(double rhs) {
    _ctype = leq;
    _rhs = rhs;
    return *this;
}

Constraint& Constraint::operator=(double rhs) {
    _ctype = eq;
    _rhs = rhs;
    return *this;
}

Constraint& Constraint::operator>=(double rhs) {
    _ctype = geq;
    _rhs = rhs;
    return *this;
}

//Constraint& Constraint::operator<=(int rhs) {
//    _ctype = leq;
//    _rhs = rhs;
//    return *this;
//}
//
//Constraint& Constraint::operator=(int rhs) {
//    _ctype = eq;
//    _rhs = rhs;
//    return *this;
//}
//
//Constraint& Constraint::operator>=(int rhs) {
//    _ctype = geq;
//    _rhs = rhs;
//    return *this;
//}


/* Accessors */

int Constraint::get_idx() const{
    return _idx;
};


int Constraint::get_type() const{
    return _ctype;
};

int Constraint::get_ftype() const{
    return _ftype;
};

/* Modifiers */
void Constraint::set_idx(int idx){
    _idx = idx;
};

Constraint& Constraint::operator=(const Function& f){
    this->reset();
    *this+=f;
    return *this;
}

/* Output */
void Constraint::print(){
    cout << _name << " | idx = " << get_idx() << " : ";
    
    this->Function::print(false);
    switch (_ctype) {
        case leq:
            cout << " <= ";
            break;
        case geq:
            cout << " >=  ";
            break;
        case eq:
            cout << " = ";
            break;
        default:
            break;
    }
//    printf("%f;\n", _rhs);
    cout << _rhs << ";\n";

};
