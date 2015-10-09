//
//  meta_var.h
//  PowerTools++
//
//  Created by Hassan on 12/05/2015.
//  Copyright (c) 2015 NICTA. All rights reserved.
//

#ifndef __PowerTools____meta_var__
#define __PowerTools____meta_var__

#include <stdio.h>
#include "var_.h"

class meta_var:public var_{
public:
    var_* var;
    meta_var();
    meta_var(Model* model);
    meta_var(string name, Model* model);
    meta_var& operator=(var_& v);
    
};
#endif /* defined(__PowerTools____meta_var__) */
