//
//  meta_Constraint.cpp
//  PowerTools++
//
//  Created by Hassan on 12/05/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#include "PowerTools++/meta_Constraint.h"


meta_Constraint::meta_Constraint():Constraint(){_nb_concrete = 0;};


meta_Constraint& meta_Constraint::operator=(const Function& f){
    this->reset();
    *this+=f;
    return *this;
}

meta_Constraint& meta_Constraint::operator<=(double rhs) {
    _ctype = leq;
    _rhs = rhs;
    return *this;
}

meta_Constraint& meta_Constraint::operator=(double rhs) {
    _ctype = eq;
    _rhs = rhs;
    return *this;
}

meta_Constraint& meta_Constraint::operator>=(double rhs) {
    _ctype = geq;
    _rhs = rhs;
    return *this;
}
