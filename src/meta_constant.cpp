//
//  meta_constant.cpp
//  PowerTools++
//
//  Created by Hassan on 12/05/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#include "PowerTools++/meta_constant.h"
#include "PowerTools++/Model.h"

meta_constant::meta_constant(Model* model):var_(){
    _type = constant;
    val = 0;
    model->addMetaConstant(*this);
    _model = model;
};

meta_constant::meta_constant(string name, Model* model):meta_constant(model){
    _name = name;
}

meta_constant& meta_constant::operator=(double v){
    val = v;
    return *this;
}