//
//  meta_var.cpp
//  PowerTools++
//
//  Created by Hassan on 12/05/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#include "PowerTools++/meta_var.h"
#include "PowerTools++/Model.h"

meta_var::meta_var(Model* model):var_(){
    var = NULL;
    model->addMetaVar(*this);
    _model = model;
};

meta_var::meta_var(string name, Model* model):meta_var(model){
    _name = name;
}

meta_var::meta_var():var_(){
    var = NULL;
};

meta_var& meta_var::operator=(var_& v){
    var = &v;
    return *this;
}
